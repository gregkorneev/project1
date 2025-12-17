import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt

plt.rcParams["font.family"] = "DejaVu Sans"
plt.rcParams["axes.unicode_minus"] = False



def main():
    in_csv = "results/factor_loadings.csv"
    out_dir = "visualizations"
    out_png = os.path.join(out_dir, "factor_loadings_heatmap.png")

    os.makedirs(out_dir, exist_ok=True)

    df = pd.read_csv(in_csv, index_col=0)  # Feature в индекс

    fig, ax = plt.subplots(figsize=(10, 6))
    im = ax.imshow(df.values, aspect="auto")

    ax.set_title("Factor loadings (features x factors)")
    ax.set_xticks(range(len(df.columns)))
    ax.set_xticklabels(df.columns, rotation=30, ha="right")
    ax.set_yticks(range(len(df.index)))
    ax.set_yticklabels(df.index)

    cbar = plt.colorbar(im, ax=ax)
    cbar.set_label("Loading")

    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close(fig)

    print(f"Saved: {out_png}")


if __name__ == "__main__":
    main()
