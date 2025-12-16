import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt

plt.rcParams["font.family"] = "DejaVu Sans"
plt.rcParams["axes.unicode_minus"] = False



def main():
    in_csv = "results/correlation_matrix.csv"
    out_dir = "visualizations"
    out_png = os.path.join(out_dir, "correlation_heatmap.png")

    os.makedirs(out_dir, exist_ok=True)

    # В CSV первая колонка называется Feature (это индекс строк)
    df = pd.read_csv(in_csv, index_col=0)

    fig, ax = plt.subplots(figsize=(10, 8))
    im = ax.imshow(df.values, aspect="auto")

    ax.set_title("Корреляционная матрица признаков")
    ax.set_xticks(range(len(df.columns)))
    ax.set_xticklabels(df.columns, rotation=90)
    ax.set_yticks(range(len(df.index)))
    ax.set_yticklabels(df.index)

    cbar = plt.colorbar(im, ax=ax)
    cbar.set_label("Коэффициент корреляции")

    plt.tight_layout()
    plt.savefig(out_png, dpi=200)
    plt.close(fig)

    print(f"Saved: {out_png}")


if __name__ == "__main__":
    main()
