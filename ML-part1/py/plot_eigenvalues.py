import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt

plt.rcParams["font.family"] = "DejaVu Sans"
plt.rcParams["axes.unicode_minus"] = False



def main():
    in_csv = "results/eigenvalues.csv"
    out_dir = "visualizations"
    out_png = os.path.join(out_dir, "eigenvalues_explained.png")

    os.makedirs(out_dir, exist_ok=True)

    df = pd.read_csv(in_csv)

    # Твой CSV пишет: factor,eigenvalue,explained,cumulative
    # explained/cumulative у тебя в долях (0..1), переведём в проценты для графика
    if not {"factor", "eigenvalue", "explained", "cumulative"}.issubset(set(df.columns)):
        raise ValueError(f"Unexpected columns in {in_csv}: {list(df.columns)}")

    df = df.sort_values("factor")
    x = df["factor"].astype(int)
    explained_pct = df["explained"] * 100.0
    cumulative_pct = df["cumulative"] * 100.0

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.bar(x, explained_pct)
    ax.plot(x, cumulative_pct, marker="o")

    ax.set_title("Explained variance by factors")
    ax.set_xlabel("Factor")
    ax.set_ylabel("Explained variance (%)")
    ax.set_xticks(x)

    ax.set_ylim(0, 105)
    ax.grid(True, axis="y", linestyle="--", linewidth=0.5)

    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close(fig)

    print(f"Saved: {out_png}")


if __name__ == "__main__":
    main()
