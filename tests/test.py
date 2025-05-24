#!/usr/bin/env python3
import ctypes
import json
import sys
import argparse
import os

# --- Конфигурация ---
LIBRARY_PATH = "..build/lib/librdbcompare.so"
DEBUG_DISABLE_FREE = False 

# --- Инициализация ctypes и загрузка библиотеки ---
try:
    librdb = ctypes.CDLL(LIBRARY_PATH)
except OSError as e:
    print(f"Ошибка: Не удалось загрузить библиотеку '{LIBRARY_PATH}'.", file=sys.stderr)
    print(f"Детали ошибки: {e}", file=sys.stderr)
    sys.exit(1)


librdb.fetch_package_list.restype = ctypes.POINTER(ctypes.c_char)
librdb.fetch_package_list.argtypes = [ctypes.c_char_p]

librdb.compare_packages.restype = ctypes.POINTER(ctypes.c_char)
librdb.compare_packages.argtypes = [ctypes.c_char_p, ctypes.c_char_p]


libc = None
if not DEBUG_DISABLE_FREE:
    try:
        libc = ctypes.CDLL(None) 
        libc.free.argtypes = [ctypes.c_void_p]
    except Exception as e:
        print(f"Предупреждение: Не удалось загрузить системную free. Утечки памяти возможны.", file=sys.stderr)
        print(f"Детали: {e}", file=sys.stderr)
        libc = None

def _free_c_ptr(c_ptr):

    if not DEBUG_DISABLE_FREE and libc and c_ptr: 
        libc.free(c_ptr)
    elif DEBUG_DISABLE_FREE and c_ptr: 
        print(f"DEBUG: Память по адресу {c_ptr} не была освобождена (DEBUG_DISABLE_FREE = True)", file=sys.stderr)

def fetch_data_from_c(branch_name: str) -> str | None:

    print(f"Запрос пакетов для ветки '{branch_name}'...", file=sys.stderr)
    c_result_ptr = librdb.fetch_package_list(branch_name.encode('utf-8'))

    if not c_result_ptr:
        print(f"Ошибка: fetch_package_list вернула пустой указатель для '{branch_name}'.", file=sys.stderr)
        return None

    try:

        json_bytes = ctypes.string_at(c_result_ptr) 
        json_data = json_bytes.decode('utf-8')
        return json_data
    except UnicodeDecodeError as e:
        print(f"Ошибка: Не удалось декодировать ответ от C-библиотеки для '{branch_name}'.", file=sys.stderr)
        print(f"Детали: {e}", file=sys.stderr)
        return None
    finally:
        _free_c_ptr(c_result_ptr) 

def compare_data_from_c(branch1_json: str, branch2_json: str) -> str | None:
 
    print(f"Выполнение сравнения пакетов...", file=sys.stderr)
    c_result_ptr = librdb.compare_packages(branch1_json.encode('utf-8'), branch2_json.encode('utf-8'))

    if not c_result_ptr:
        print("Ошибка: compare_packages вернула пустой указатель.", file=sys.stderr)
        return None

    try:
        
        json_bytes = ctypes.string_at(c_result_ptr)
        json_data = json_bytes.decode('utf-8')
        return json_data
    except UnicodeDecodeError as e:
        print(f"Ошибка: Не удалось декодировать ответ от C-библиотеки при сравнении.", file=sys.stderr)
        print(f"Детали: {e}", file=sys.stderr)
        return None
    finally:
        _free_c_ptr(c_result_ptr) 

# --- Вспомогательная функция для форматированного вывода результатов сравнения ---
def print_comparison_results(results_json: dict, categories: list[str]):
    if not results_json or "architectures" not in results_json:
        print("Ошибка: Некорректный формат JSON-результата сравнения.", file=sys.stderr)
        return

    architectures = results_json.get("architectures", {})

    if not architectures:
        print("Нет данных для сравнения по архитектурам.")
        return

    for arch, data in architectures.items():
        print(f"\n--- Архитектура: {arch} ---")
        
        has_printed_category_for_arch = False
        for category in categories:
            items = data.get(category)
            if items is not None:
                if items:
                    print(f"\n  Категория: {category.replace('_', ' ').capitalize()}")
                    has_printed_category_for_arch = True
                    if category == "branch1_newer":
                        for item in items:
                            name = item.get("name", "N/A")
                            b1_ver = item.get("branch1_version_release", "N/A")
                            b2_ver = item.get("branch2_version_release", "N/A")
                            print(f"    - {name}: B1({b1_ver}) > B2({b2_ver})")
                    else: # branch1_only, branch2_only
                        for item in items:
                            print(f"    - {item}")
                else:
                    if len(categories) == 1 or "all" in categories:
                        print(f"\n  Категория: {category.replace('_', ' ').capitalize()} - Нет различий.")
                        has_printed_category_for_arch = True
        
        if not has_printed_category_for_arch and len(categories) > 0:
            print("  Для этой архитектуры нет различий в запрошенных категориях.")


# --- Разбор аргументов командной строки ---
parser = argparse.ArgumentParser(
    description="Инструмент для сравнения списков пакетов ALT Linux между ветками.",
    formatter_class=argparse.RawTextHelpFormatter
)
parser.add_argument(
    "branch1",
    nargs="?",
    default="sisyphus",
    help="Имя первой ветки (по умолчанию: sisyphus)"
)
parser.add_argument(
    "branch2",
    nargs="?",
    default="p10",
    help="Имя второй ветки (по умолчанию: p10)"
)
parser.add_argument(
    "-c", "--category",
    choices=["branch1_only", "branch2_only", "branch1_newer", "all"],
    default="all",
    help=(
        "Категория результатов для вывода:\n"
        "  branch1_only   - пакеты только в первой ветке\n"
        "  branch2_only   - пакеты только во второй ветке\n"
        "  branch1_newer  - пакеты, новее в первой ветке\n"
        "  all            - все категории (по умолчанию)"
    )
)
parser.add_argument(
    "-r", "--raw-json",
    action="store_true",
    help="Вывести необработанный JSON-ответ сравнения без форматирования."
)

args = parser.parse_args()

# --- Основная логика скрипта ---


branch1_data_json_str = fetch_data_from_c(args.branch1)
if branch1_data_json_str is None:
    sys.exit(1)


branch2_data_json_str = fetch_data_from_c(args.branch2)
if branch2_data_json_str is None:
    sys.exit(1)


comparison_json_str = compare_data_from_c(branch1_data_json_str, branch2_data_json_str)
if comparison_json_str is None:
    sys.exit(1)

if args.raw_json:
    print(comparison_json_str)
else:
    try:
        comparison_results = json.loads(comparison_json_str)
        if args.category == "all":
            selected_categories = ["branch1_only", "branch2_only", "branch1_newer"]
        else:
            selected_categories = [args.category]

        print_comparison_results(comparison_results, selected_categories)

    except json.JSONDecodeError as e:
        print(f"Ошибка: Не удалось разобрать JSON-ответ сравнения.", file=sys.stderr)
        print(f"Детали: {e}", file=sys.stderr)
        print(f"Необработанный JSON (частично): {comparison_json_str[:500]}...", file=sys.stderr)
        sys.exit(1)

sys.exit(0)