import random

def quadratic_probe_insert(hash_table, key):
    """Вставка ключа с квадратичным пробированием: h(k,i)=h(k)+i+i^2"""
    M = len(hash_table)
    h0 = key % M
    i = 0
    while i < M:
        idx = (h0 + i + i*i) % M
        if hash_table[idx] is None:
            hash_table[idx] = key
            return i
        i += 1
    return i

def cubic_probe_insert(hash_table, key):
    """Вставка ключа с кубическим пробированием: h(k,i)=h(k)+i+i^2+i^3"""
    M = len(hash_table)
    h0 = key % M
    i = 0
    while i < M:
        idx = (h0 + i + i*i + i*i*i) % M
        if hash_table[idx] is None:
            hash_table[idx] = key
            return i
        i += 1
    return i

def simulate_probing(probe_func, table_size, num_keys):
    """Запускаем последовательную вставку num_keys ключей в таблицу размером table_size.
       Возвращаем среднее число коллизий на вставку."""
    hash_table = [None] * table_size
    total_collisions = 0
    for _ in range(num_keys):
        key = random.randint(0, 10**6)
        collisions = probe_func(hash_table, key)
        total_collisions += collisions
    return total_collisions / num_keys

if __name__ == '__main__':
    table_sizes = [101, 1009, 10007]
    load_factors = [0.5, 0.7, 0.9]

    for size in table_sizes:
        for lf in load_factors:
            num_keys = int(size * lf)
            avg_quad = simulate_probing(quadratic_probe_insert, size, num_keys)
            avg_cubic = simulate_probing(cubic_probe_insert, size, num_keys)
            print(f"Размер таблицы: {size}, Коэффициент заполнения: {lf}")
            print(f"  Квадратичное пробирование - среднее число коллизий: {avg_quad:.2f}")
            print(f"  Кубическое пробирование  - среднее число коллизий: {avg_cubic:.2f}\n")
