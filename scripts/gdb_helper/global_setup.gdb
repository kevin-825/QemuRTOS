# ==============================================================================
# PART 1: GLOBAL SETUP (UI, History, Logging)
# ==============================================================================

# --- Core Features ---
set pagination off
set output-radix 16
set print pretty on
set remotetimeout 250
set confirm off

# --- History Management ---
set history save on
set history size unlimited
set history remove-duplicates unlimited
set history filename ./scripts/gdb_helper/.gdb_history

# --- Logging Setup ---
set logging enabled off
set logging file ./scripts/gdb_helper/gdb_log.txt
set logging overwrite on  
set logging enabled on

# --- TUI Visuals ---
set style tui-border foreground green
set style line-number foreground cyan
set tui mouse-events on

# Flush any stale registers from previous sessions
maintenance flush register-cache
