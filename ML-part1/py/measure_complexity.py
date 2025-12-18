import os
import time
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def standardize_matrix(X: np.ndarray) -> np.ndarray:
    """Z-score стандартизация по столбцам."""
    mean = X.mean(axis=0)
    std = X.std(axis=0, ddof=0)
    std[std == 0] = 1.0
    return (X - mean) / std


def pca_eigendecomposition(X: np.ndarray) -> np.ndarray:
    """
    PCA через разложение корреляционной матрицы.
    Возвращает собственные значения (по убыванию).
    """
    # корреляционная матрица: m x m
    corr = np.corrcoef(X, rowvar=False)

    # собственные значения/векторы
    eigenvalues, _ = np.linalg.eigh(corr)

    # сортировка по убыванию
    eigenvalues = np.sort(eigenvalues)[::-1]
    return eigenvalues


def measure_times(X_full: np.ndarray, sizes: list[int], repeats: int) -> list[dict]:
    results = []
    n_max = X_full.shape[0]

    rng = np.random.default_rng(42)

    for n in sizes:
        n = min(n, n_max)
        # фиксируем подвыборку, чтобы сравнение было честным
        idx = rng.choice(n_max, size=n, replace=False)
        X = X_full[idx]

        # прогрев (разово)
        _ = pca_eigendecomposition(X)

        times = []
        for _ in range(repeats):
            t0 = time.perf_counter()
            _ = pca_eigendecomposition(X)
            t1 = time.perf_counter()
            times.append((t1 - t0) * 1000.0)  # ms

        results.append({
            "n": int(n),
            "m": int(X.shape[1]),
            "time_ms_mean": float(np.mean(times)),
            "time_ms_std": float(np.std(times, ddof=1) if repeats > 1 else 0.0),
            "repeats": int(repeats)
        })

    return results


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", required=True, help="Путь к CSV файлу с данными")
    parser.add_argument("--target", default="", help="Имя целевого столбца (если есть) — будет исключён")
    parser.add_argument("--sizes", default="200,500,1000,2000,5000,10000",
                        help="Список размеров n через запятую")
    parser.add_argument("--repeats", type=int, default=5, help="Сколько повторов замера для каждого n")
    parser.add_argument("--results_dir", default="results", help="Папка для CSV результатов")
    parser.add_argument("--viz_dir", default="visualizations", help="Папка для графиков")
    args = parser.parse_args()

    os.makedirs(args.results_dir, exist_ok=True)
    os.makedirs(args.viz_dir, exist_ok=True)

    df = pd.read_csv(args.csv)

    # оставляем только числовые признаки
    if args.target and args.target in df.columns:
        df = df.drop(columns=[args.target])

    df_num = df.select_dtypes(include=[np.number]).copy()

    if df_num.shape[1] < 2:
        raise ValueError("Недостаточно числовых признаков для факторного анализа / PCA.")

    # удаляем строки с пропусками (для честного замера)
    df_num = df_num.dropna()

    X = df_num.to_numpy(dtype=float)

    # стандартизация
    X = standardize_matrix(X)

    sizes = [int(s.strip()) for s in args.sizes.split(",") if s.strip()]
    measurements = measure_times(X, sizes=sizes, repeats=args.repeats)

    out_csv = os.path.join(args.results_dir, "complexity_measurements.csv")
    pd.DataFrame(measurements).to_csv(out_csv, index=False, encoding="utf-8")

    # график
    xs = [r["n"] for r in measurements]
    ys = [r["time_ms_mean"] for r in measurements]
    yerr = [r["time_ms_std"] for r in measurements]

    plt.figure()
    plt.errorbar(xs, ys, yerr=yerr, fmt='o-')
    plt.xlabel("Количество объектов n")
    plt.ylabel("Время выполнения, мс")
    plt.title("Зависимость времени факторного анализа от объёма данных")
    plt.grid(True)
    out_png = os.path.join(args.viz_dir, "factor_analysis_time_vs_n.png")
    plt.savefig(out_png, dpi=200, bbox_inches="tight")

    print("Готово.")
    print(f"CSV:  {out_csv}")
    print(f"PNG:  {out_png}")


if __name__ == "__main__":
    main()
