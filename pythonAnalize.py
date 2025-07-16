import io
import pandas as pd
import matplotlib.pyplot as plt

FIGSIZE = (20, 14)     # <<< change this to control EVERY plot’s width & height
# ──────────────────────────────────────────────────────────────────────────
FIG_W, FIG_H = 10, 8      # inches   ←  tweak just these two numbers
DPI        = 100          # dots‑per‑inch (leave None to use Matplotlib’s default)
# ──────────────────────────────────────────────────────────────────────────

def new_fig(title: str):
    """Create a figure and enlarge the *window* as well as the canvas."""
    fig = plt.figure(figsize=(FIG_W, FIG_H), dpi=DPI)
    fig.canvas.manager.set_window_title(title)
    fig.canvas.manager.set_window_title(title)  # window caption
    fig.suptitle(title, fontsize=14)    
    # Try to resize the outer window (works for TkAgg, Qt[5|6]Agg, WXAgg, …)
    mng = plt.get_current_fig_manager()
    try:
        mng.resize(int(FIG_W * DPI), int(FIG_H * DPI))
    except Exception:
        # Some back‑ends (e.g. MacOSX) don’t expose resize(); ignore.
        pass
    return fig

# --- paste the CSV into a triple‑quoted string ------------------------------
df = pd.read_csv("timingData.csv", skipinitialspace=True)
# --- derive the combined‑time column ----------------------------------------
ops = ["adding_extra_points", "create_unconstraint",
       "force_constraints", "delete_outside_tri"]
df["ops_sum"] = df[ops].sum(axis=1)
df["create_unconstraint_m"] = df["create_unconstraint"] / 1_000_000
df["ops_sum_m"]            = df["ops_sum"]            / 1_000_000
df["force_constraints_m"]  = df["force_constraints"]  / 1_000_000
df["delete_outside_tri"]   = df["delete_outside_tri"]  / 1_000_000

# ─── 1) n_points vs create_unconstraint ───────────────────────────────────
new_fig("Create Unconstraint vs n points")
plt.scatter(df["n_points"], df["create_unconstraint_m"])
plt.xlabel("n_points")
plt.ylabel("create unconstraint (s)")

# ─── 2) n_points vs ops_sum ───────────────────────────────────────────────
new_fig("Total time vs n of Points")
plt.scatter(df["n_points"], df["ops_sum_m"])
plt.xlabel("n points")
plt.ylabel("total time (s)")

# ─── 3) total_tri vs ops_sum ──────────────────────────────────────────────
new_fig("Time to delete outside tri vs Total Triangles")
plt.scatter(df["total_tri"], df["delete_outside_tri"])
plt.xlabel("total tri")
plt.ylabel("time (s)")

# ─── 4) n_edges vs force_constraints ─────────────────────────────────────
new_fig("time to force constraints vs Number of constrained Edges created")
plt.scatter(df["n_edges"], df["force_constraints_m"])
plt.xlabel("n edges")
plt.ylabel("time to force constraints (s)")

plt.show()
