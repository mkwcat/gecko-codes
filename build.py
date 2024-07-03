import sys, os, struct, subprocess, string
from source.nsmbw.lib_address_maps import *

devkitppc = os.environ.get("DEVKITPPC")
path_cc = os.path.join(devkitppc, "bin", "powerpc-eabi-gcc")
path_objcopy = os.path.join(devkitppc, "bin", "powerpc-eabi-objcopy")

ccflags = '-fPIC -fno-rtti -ffreestanding -nodefaultlibs -nostdlib -fno-unwind-tables -fno-exceptions \
-fmerge-all-constants -ffunction-sections -fdata-sections -fno-short-enums'

warning_flags = '-Wno-attribute-alias'

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
    sfile = open(os.path.join(path_build, "gct_out." + revision + ".s"), "w")
    sfile.write(process_asm(mapper_from, asmdata, address_map[revision]))
    sfile.close()

    subprocess.run([path_cc, "-mregnames", os.path.join(path_build, "gct_out." + revision + ".s"), "-nostdlib", "-ffreestanding", "-nodefaultlibs", "-o" + os.path.join(path_build, "gct_out." + revision + ".elf"), "-e", "0"])
    subprocess.run([path_objcopy, "-O", "binary", os.path.join(path_build, "gct_out." + revision + ".elf"), os.path.join(path_build, "gct_out." + revision + ".bin")])

def process_nsmbu(path_build, asmdata):
    sfile = open(os.path.join(path_build, "gct_out.s"), "w")
    sfile.write(process_asm(None, asmdata, None))
    sfile.close()

    subprocess.run([path_cc, "-mregnames", os.path.join(path_build, "gct_out.s"), "-nostdlib", "-ffreestanding", "-nodefaultlibs", "-o" + os.path.join(path_build, "gct_out.elf"), "-e", "0"])
    subprocess.run([path_objcopy, "-O", "binary", os.path.join(path_build, "gct_out.elf"), os.path.join(path_build, "gct_out.bin")])

def add_readme_code(readme, path, revision):
    binary_file = open(path, "rb")
    raw = binary_file.read()
    binary_file.close()

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

    result = subprocess.run([path_cc, "-S", path_src, "-Isource", "-o-", "-D__POWERPC__"] + ccflags.split(" ") + warning_flags.split(" "), stdout=subprocess.PIPE)
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

# # New Super Mario Bros. U
run_build("nsmbu", "Lift-Anything-NSMBU")
run_build("nsmbu", "Save-Anytime")
