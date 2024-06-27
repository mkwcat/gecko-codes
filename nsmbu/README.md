# New Super Mario Bros. U

Gecko codes are not designed for Wii U games, but I found any existing methods too
complex for my liking, and I had trouble setting them up. As a result, I created a
simple system that is maybe more complex for the user than anything else.

## Applying gecko codes to NSMBU

The current code(s) are very limited and only support the specific version of NSMBU that I have. Specifically the US NSMBU + NSLU bundle version (you know the one that includes like 12 GB of MP4s). The script will be patching that version's main executable `red-pro2.rpx` file, so you'll need a clean copy of the file, as well as a way to replace it afterwards.

To patch your game's RPX file:
- Decompress the RPX file into a regular ELF (named like `red-pro2-base.elf`) using a tool such as [wiiurpxtool](https://github.com/0CBH0/wiiurpxtool/releases).
- Use the Python script `patch-nsmbu.py` located in the current directory to patch the game, suppling the base ELF, MD file containing the gecko code, and output ELF.
 - Example: `python patch-nsmbu.py red-pro2-base.elf Lift-Anything.md red-pro2-patched.rpx`
- Recompress the output ELF file back to RPX and replace the `red-pro2.rpx` file in the game.

If done correctly it should work!!
