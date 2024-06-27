import sys, os, struct, subprocess, string

devkitppc = os.environ.get("DEVKITPPC")
path_cc = os.path.join(devkitppc, "bin", "powerpc-eabi-gcc")
path_objcopy = os.path.join(devkitppc, "bin", "powerpc-eabi-objcopy")

game_base = open(sys.argv[1], "rb").read()
game_base = bytearray(game_base)

md = open(sys.argv[2], "r").readlines()

# find line starting with "```hex"
hex_start = -1
for i in range(len(md)):
    if md[i].startswith("```hex"):
        hex_start = i
        break

# decode hex data
if hex_start == -1:
    print("No code found in MD file")
    sys.exit(1)

gct = bytearray()
for i in range(hex_start+1, len(md)):
    if md[i].startswith("```"):
        break
    gct += bytes.fromhex(md[i][:8])
    gct += bytes.fromhex(md[i][9:9+8])

if len(gct) == 0:
    print("No code found in MD file")
    sys.exit(1)

if len(gct) % 8 != 0:
    print("Code length is not a multiple of 8")
    sys.exit(1)

# patch the game binary
# read the section table
section_table_offset = struct.unpack(">I", game_base[0x20:0x24])[0]
section_table_size = struct.unpack(">H", game_base[0x30:0x32])[0]

patch_start = struct.unpack(">I", game_base[0x9C:0xA0])[0]
code_addr = patch_start
code_offset = struct.unpack(">I", game_base[0xA0:0xA4])[0]
code_len = struct.unpack(">I", game_base[0xA4:0xA8])[0]
patch_start += code_len

i = 0
while i < len(gct):
    c = gct[i]
    if c == 0xC2:
        hook_addr = (struct.unpack(">I", gct[i+0:i+4])[0] & 0x00FFFFFF) + 0x02000000
        instr_count = struct.unpack(">I", gct[i+4:i+8])[0] * 2

        hook_offset = hook_addr - code_addr + code_offset
        cur_patch = patch_start + i + 8
        cur_patch_end = cur_patch + instr_count*4 - 4

        hook_branch = 0x48000000 | ((cur_patch - hook_addr) & 0x03FFFFFC)
        hook_branch_back = 0x48000000 | ((hook_addr + 4 - cur_patch_end) & 0x03FFFFFC)

        game_base[hook_offset:hook_offset+4] = struct.pack(">I", hook_branch)
        i += 8 + instr_count*4
        gct[i-4:i] = struct.pack(">I", hook_branch_back)
        
    elif c == 0x04:
        hook_addr = (struct.unpack(">I", gct[i+0:i+4])[0] & 0x00FFFFFF) + 0x02000000
        hook_value = struct.unpack(">I", gct[i+4:i+8])[0]

        hook_offset = hook_addr - code_addr + code_offset
        game_base[hook_offset:hook_offset+4] = struct.pack(">I", hook_value)
        i += 8

# expand code section and write
game_base = game_base[:code_offset + code_len] + gct + game_base[code_offset + code_len:]
game_base[0xA4:0xA8] = struct.pack(">I", len(gct) + code_len)

# shift all sections after the code section
for i in range(1, section_table_size):
    offset = section_table_offset + i * 0x28
    section_offset = struct.unpack(">I", game_base[offset+0x10:offset+0x14])[0]
    if section_offset > code_offset+code_len:
        section_offset += len(gct)
        game_base[offset+0x10:offset+0x14] = struct.pack(">I", section_offset)

# write the new game binary
game_base = bytes(game_base)
game_out = open(sys.argv[3], "wb")
game_out.write(game_base)
game_out.close()