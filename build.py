import sys, os, struct, subprocess, string
from source.nsmbw.lib_address_maps import *

devkitppc = os.environ.get("DEVKITPPC")
path_cc = os.path.join(devkitppc, "bin", "powerpc-eabi-gcc")
path_objcopy = os.path.join(devkitppc, "bin", "powerpc-eabi-objcopy")

ccflags = '-fPIC -Os -fno-rtti -ffreestanding -nodefaultlibs -nostdlib -fno-unwind-tables -fno-exceptions \
-fmerge-all-constants -ffunction-sections -fdata-sections -fno-short-enums -fshort-wchar -std=gnu++2b'
ldflags = '-Wl,--gc-sections -n'

warning_flags = '-Wno-attribute-alias -Wno-invalid-offsetof'

def process_asm(mapper_from, asmdata, region):
    offset = -1
    while True:
        offset = asmdata.find("PORT", offset + 1)
        if offset == -1:
            break
        # make sure it is its own token
        pc = " "
        nc = " "
        try:
            pc = asmdata[offset - 1]
            nc = asmdata[offset + 4]
        except:
            pass

        if pc.isalnum() or pc == "_":
            continue

        if nc.isalnum() or nc == "_":
            continue

        asmdata = asmdata[:offset] + asmdata[offset+4:]

        if mapper_from is None or region is None or region == "P1":
            continue

        npos = 0
        while nc != "(":
            assert(nc == " " or nc == "\r" or nc == "\n")
            npos += 1
            nc = asmdata[offset+npos]


        npos += 1
        nc = asmdata[offset+npos]
        while nc.isdigit() == False:
            assert(nc == " " or nc == "\r" or nc == "\n")
            npos += 1
            nc = asmdata[offset+npos]

        num_start = npos

        if asmdata[offset+num_start:offset+num_start+2] == "0x":
            npos += 2
            nc = asmdata[offset+npos]
            while nc in string.hexdigits:
                npos += 1
                nc = asmdata[offset+npos]
        else:
            npos += 1
            nc = asmdata[offset+npos]
            while nc.isdigit():
                npos += 1
                nc = asmdata[offset+npos]

        assert(nc.isalnum() == False and nc != "_")
        num_end = npos

        num_string = asmdata[offset+num_start:offset+num_end]
        address = int(num_string, 0)
        port_address = map_addr_from_to(mapper_from, region, address)

        asmdata = asmdata[:offset+num_start] + str(port_address) + asmdata[offset+num_end:]

    return asmdata

def process_nsmbw(path_build, asmdata, mapper_from, address_map, revision):
    sfile = open(os.path.join(path_build, "gct_out." + revision + ".s"), "wb")
    sfile.write(process_asm(mapper_from, asmdata, address_map[revision]).encode("utf-8"))
    sfile.close()

    subprocess.run([path_cc, "-mregnames", os.path.join(path_build, "gct_out." + revision + ".s"), "-nostdlib", "-ffreestanding", "-nodefaultlibs", "-o" + os.path.join(path_build, "gct_out." + revision + ".elf"), "-Tgct.ld", "-lgcc"] + ldflags.split(" "))
    subprocess.run([path_objcopy, "-O", "binary", os.path.join(path_build, "gct_out." + revision + ".elf"), os.path.join(path_build, "gct_out." + revision + ".bin")])

def process_mkw(path_build, asmdata):
    sfile = open(os.path.join(path_build, "gct_out.s"), "wb")
    sfile.write(process_asm(None, asmdata, None).encode("utf-8"))
    sfile.close()

    subprocess.run([path_cc, "-mregnames", os.path.join(path_build, "gct_out.s"), "-nostdlib", "-ffreestanding", "-nodefaultlibs", "-o" + os.path.join(path_build, "gct_out.elf"), "-Tgct.ld", "-lgcc"] + ldflags.split(" "))
    subprocess.run([path_objcopy, "-O", "binary", os.path.join(path_build, "gct_out.elf"), os.path.join(path_build, "gct_out.bin")])

def process_nsmbu(path_build, asmdata):
    sfile = open(os.path.join(path_build, "gct_out.s"), "wb")
    sfile.write(process_asm(None, asmdata, None).encode("utf-8"))
    sfile.close()

    subprocess.run([path_cc, "-mregnames", os.path.join(path_build, "gct_out.s"), "-nostdlib", "-ffreestanding", "-nodefaultlibs", "-o" + os.path.join(path_build, "gct_out.elf"), "-Tgct.ld"])
    subprocess.run([path_objcopy, "-O", "binary", os.path.join(path_build, "gct_out.elf"), os.path.join(path_build, "gct_out.bin")])

def process_cpp_gecko(data, gct_start):
    i = 0
    while i < ((len(data) - 8) - gct_start) >> 3:
        j = gct_start + (i * 8)

        words = struct.unpack(">II", data[j:j+8])
        if (words[0] & ~0x01FFFFFF) == 0xC4000000:
            words2 = struct.unpack(">II", data[j+8:j+16])
            dest = words2[0]
            branch = words2[1]

            inst = struct.unpack(">I", data[dest:dest+4])[0]
            if (inst & ~0x00FFFFFF) == 0x00000000:
                dest += 4

            origin = j + 8
            branch |= (dest - origin) & 0x03FFFFFC
            data[j] = 0xC2 | ((words[0] >> 24) & 0x01)
            data[origin] = branch >> 24
            data[origin+1] = (branch >> 16) & 0xFF
            data[origin+2] = (branch >> 8) & 0xFF
            data[origin+3] = branch & 0xFF
            data[origin+4] = 0
            data[origin+5] = 0
            data[origin+6] = 0
            data[origin+7] = 0

        if (words[0] & ~0x00FFFFFF) == 0x4E000000:
            dest = struct.unpack(">I", data[j+4:j+8])[0]
            inst = struct.unpack(">I", data[dest:dest+4])[0]
            if (inst & ~0x00FFFFFF) == 0x00000000:
                dest += 4

            origin = j + 4
            branch = 0x48000000
            branch |= (dest - origin) & 0x03FFFFFC
            data[origin] = branch >> 24
            data[origin+1] = (branch >> 16) & 0xFF
            data[origin+2] = (branch >> 8) & 0xFF
            data[origin+3] = branch & 0xFF

        # not handled but to skip the necessary amount of lines

        if (words[0] & ~0x00FFFFFF) == 0xC2000000:
            i += words[1]
        elif (words[0] & ~0x00FFFFFF) == 0xC0000000:
            i += words[1]
        elif (words[0] & ~0x00FFFFFF) == 0x06000000:
            i += (words[1] + 7) >> 3 

        i += 1

    return data


def add_readme_code(readme, path, revision):
    binary_file = open(path, "rb")
    raw = binary_file.read()
    binary_file.close()

    assert(len(raw) >= 8)
    cpp_setup_end = struct.unpack(">I", raw[-4:])[0]
    if cpp_setup_end != 0:
        raw = process_cpp_gecko(bytearray(raw), cpp_setup_end)
        cpp_setup_end >>= 3
        raw = raw[-8:-4] + struct.pack(">I", cpp_setup_end) + raw[:-8]
    else:
        raw = raw[:-8]

    # add collapsible code block named after the revision
    readme += "<details>\n"
    readme += "<summary>" + revision + "</summary>\n"
    readme += "\n```hex\n"

    while True:
        assert(len(raw) >= 8)
        readme += "{:02X}{:02X}{:02X}{:02X} {:02X}{:02X}{:02X}{:02X}\n".format(raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7])

        raw = raw[8:]
        if len(raw) == 0:
            break

    readme += "```\n"
    readme += "</details>\n\n"

    return readme

def run_build(game, name):
    path = os.path.join(game, name)

    path_src = os.path.join("source", path + ".cpp")
    path_build = os.path.join("build", path)

    if not os.path.exists(path_build):
        os.makedirs(path_build)

    tmp_ccflags = ccflags + " -Isource"
    if game == "nsmbw":
        tmp_ccflags += " -DWII=1 -Isource/wii -DNSMBW=1 -Isource/nsmbw/include -DRVL_WBC=1 -DHAS_EGG=1 -lgcc"
    elif game == "mkw":
        tmp_ccflags += " -DWII=1 -Isource/wii -DMKW=1 -Isource/mkw/include -DHAS_EGG=1 -lgcc"

    result = subprocess.run([path_cc, "-S", path_src, "-o-", "-D__POWERPC__"] + tmp_ccflags.split(" ") + warning_flags.split(" "), stdout=subprocess.PIPE)
    result.check_returncode()
    asmdata = result.stdout.decode("utf-8")

    if game == "nsmbw":
        address_map = load_address_map(open(os.path.join("source", "nsmbw", "address-map.txt"), "r"))
        mapper_from = address_map["P1"]

        process_nsmbw(path_build, asmdata, mapper_from, address_map, "P1")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "P2")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "E1")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "E2")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "J1")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "J2")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "K")
        process_nsmbw(path_build, asmdata, mapper_from, address_map, "W")
    elif game == "mkw":
        process_mkw(path_build, asmdata)
    elif game == "nsmbu":
        process_nsmbu(path_build, asmdata)

    readme = ""
    # check if the file starts with "// [Gecko]""
    source_file = open(path_src, "r")
    first_line = source_file.readline()
    if first_line.startswith("// [Gecko]"):
        title_line = source_file.readline()
        # e.g. "// $Lift Anything"
        if title_line.startswith("// $"):
            readme = "## " + title_line[4:] + "\n"

        added = False
        while True:
            line = source_file.readline()
            if not line:
                break

            if not line.startswith("// *"):
                break

            readme += line[4:]
            added = True

        if added:
            readme += "\n"

    if readme == "":
        readme = "## " + name.replace("-", " ") + "\n\n"

    if game == "nsmbw":
        readme += "This code is for New Super Mario Bros. Wii. Many revisions may be untested.\n\n"

        regions = ["P1", "P2", "E1", "E2", "J1", "J2", "K", "W"]
        rev_names = ["PAL Rev 1", "PAL Rev 2", "USA Rev 1", "USA Rev 2", "JPN Rev 1", "JPN Rev 2", "KOR", "TWN"]

        for i in range(len(regions)):
            readme = add_readme_code(readme, os.path.join("build", path, "gct_out." + regions[i] + ".bin"), rev_names[i])
    elif game == "mkw":
        readme += "This code is for Mario Kart Wii."

        readme = add_readme_code(readme, os.path.join("build", path, "gct_out.bin"), "PAL")
    elif game == "nsmbu":
        readme += "This code is for New Super Mario Bros. U. Please read the [README](README.md) in the current directory for how to use this code.\n\n"

        readme = add_readme_code(readme, os.path.join("build", path, "gct_out.bin"), "USA NSMBU + NSLU")

    if not os.path.exists(game):
        os.makedirs(game)

    readme_file = open(path + ".md", "w")
    readme_file.write(readme)
    readme_file.close()

# New Super Mario Bros. Wii
run_build("nsmbw", "Lift-Anything")
run_build("nsmbw", "Player-1-Can-Change-Character")
run_build("nsmbw", "Duplicate-Anything")
run_build("nsmbw", "Random-Worldmap-Anim")
run_build("nsmbw", "Destroy-Any-Tile")
run_build("nsmbw", "Fully-Open-Areas")
run_build("nsmbw", "Connect-All-Pipes")
run_build("nsmbw", "Cycle-Through-Powerups")
run_build("nsmbw", "Allow-Movement-During-Cutscenes")
run_build("nsmbw", "Fall-Damage")
run_build("nsmbw", "Death-Messages")
run_build("nsmbw", "Load-More-Gecko-Codes")
run_build("nsmbw", "World-Map-Expansion-Support")
run_build("nsmbw", "GameCube-Controller-Support")
run_build("nsmbw", "Save-Anytime")
run_build("nsmbw", "Exit-Course-Anytime")
run_build("nsmbw", "Disable-Powerup-Change-Pause")
run_build("nsmbw", "Freeze-Any-Enemy")
run_build("nsmbw", "No-Projectile-Limits")
run_build("nsmbw", "Midair-Crouching")

# # New Super Mario Bros. U
run_build("nsmbu", "Lift-Anything-NSMBU")
run_build("nsmbu", "Save-Anytime")
run_build("nsmbu", "Arbitrary-Course-Load")
run_build("nsmbu", "Duplicate-Anything-NSMBU")

# Mario Kart Wii
run_build("mkw", "Load-More-Gecko-Codes")
run_build("mkw", "Balance-Board-Support")