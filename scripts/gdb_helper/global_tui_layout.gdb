# Layout 0: Source Level (Focus on code)
# Top: Source (large), Bottom: Command window
tui new-layout layout0 src 1 status 1 cmd 1

# Layout 1: Assembly Level (Focus on CPU instructions)
# Top: Registers, Middle: Assembly, Bottom: Command window
tui new-layout layout1 regs 1 asm 1 status 1 cmd 1


# Layout 2: Mixed Mode (The "Pro" view)
# Top: Source and Assembly side-by-side, Bottom: Command window

# Row 1: Registers (Full Width)
# Row 2: Source (Left) and Assembly (Right) side-by-side
# Row 3: Command Window (Implicitly at the bottom)
tui new-layout layout2 \
    regs 1 \
    {src 1 asm 1} 2 \
    status 1 \
    cmd 1



# Switch to Source View
define l0
  layout layout0
  focus src
end

# Switch to ASM View
define l1
  layout layout1
  focus asm
end

# Switch to Mixed View
define l2
  layout layout2
  focus src
end



# Bind F1, F2, F3 to your layout macros





