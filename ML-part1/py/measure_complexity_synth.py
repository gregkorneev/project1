import os
import time
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def standardize(X: np.ndarray) -> np.ndarray:
    mean = X.mean(axis=0)
    std = X.std(axis=0, ddof=0)
    std[std == 0] = 1.0
    return (X - mean) / std


def factor_core(X: np.ndarray) -> np.ndarray:
    """
    Ядро факторного анализа по вычислительной нагрузке:
    1) корреляционная матрица m×m
    2) собственные значения/векторы (eigendecomposition)
    """
    X = standardize(X)
    corr = np.corrcoef(X, rowvar=False)          # m×m
    eigenvalues, _ = np.linalg.eigh(corr)        # O(m^3)
    return eigenvalues


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--m", type=int, default=10, help="Число признаков (в варианте 14: 10)")
    parser.add_argument("--sizes", default="200,500,1000,2000,5000,10000,20000,40000",
                        help="Размеры n через запятую")
    parser.add_argument("--repeats", type=int, default=5, help="Повторы для усреднения")
    parser.add_argument("--seed", type=int, default=42, help="Seed генератора")
    parser.add_argument("--results_dir", default="ML-part1/results", help="Папка для CSV")
    parser.add_argument("--viz_dir", default="ML-part1/visualizations", help="Папка для PNG")
    args = parser.parse_args()

    os.makedirs(args.results_dir, exist_ok=True)
    os.makedirs(args.viz_dir, exist_ok=True)

    rng = np.random.default_rng(args.seed)
    sizes = [int(s.strip()) for s in args.sizes.split(",") if s.strip()]

    rows = []
    for n in sizes:
        X = rng.normal(size=(n, args.m)).astype(float)

        # прогрев
        _ = factor_core(X)

        times = []
        for _ in range(args.repeats):
            t0 = time.perf_counter()
            _ = factor_core(X)
            t1 = time.perf_counter()
            times.append((t1 - t0) * 1000.0)

        rows.append({
            "n": n,
            "m": args.m,
            "time_ms_mean": float(np.mean(times)),
            "time_ms_std": float(np.std(times, ddof=1) if args.repeats > 1 else 0.0),
            "repeats": args.repeats
        })

    df = pd.DataFrame(rows)
    out_csv = os.path.join(args.results_dir, "complexity_measurements.csv")
    df.to_csv(out_csv, index=False, encoding="utf-8")

    # график
    plt.figure()
    plt.errorbar(df["n"], df["time_ms_mean"], yerr=df["time_ms_std"], fmt="o-")
    plt.xlabel("Количество объектов n")
    plt.ylabel("Время выполнения, мс")
    plt.title("Зависимость времени факторного анализа от объёма данных (m=10)")
    plt.grid(True)

    out_png = os.path.join(args.viz_dir, "factor_analysis_time_vs_n.png")
    plt.savefig(out_png, dpi=200, bbox_inches="tight")

    print("Готово.")
    print("CSV:", out_csv)
    print("PNG:", out_png)


if __name__ == "__main__":
    main()
