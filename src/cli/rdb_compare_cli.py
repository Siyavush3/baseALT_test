#!/usr/bin/env python3
import ctypes
import json
import sys
import argparse
import os

# --- Конфигурация ---
__version__ = "1.0.0"

def find_library_path(lib_name):

    system_lib_path = os.path.join("/usr", "lib", f"{lib_name}.so")
    if os.path.exists(system_lib_path):
        return system_lib_path

    build_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../build/lib", f"{lib_name}.so")
    if os.path.exists(build_path):
        return build_path

    try:
        from ctypes.util import find_library
        found_in_system = find_library(lib_name)
        if found_in_system:
            return found_in_system
    except ImportError:
        pass 

    return None 

LIBRARY_NAME = "librdbcompare"
LIBRARY_PATH = find_library_path(LIBRARY_NAME)

if not LIBRARY_PATH:
    print(f"Ошибка: Не удалось найти библиотеку '{LIBRARY_NAME}.so'.", file=sys.stderr)
    print("Убедитесь, что она скомпилирована (make) или установлена (sudo make install).", file=sys.stderr)
    sys.exit(1)


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
try:
    libc = ctypes.CDLL(None)
    libc.free.argtypes = [ctypes.c_void_p]
except Exception as e:
    print(f"Предупреждение: Не удалось загрузить системную free для освобождения памяти. Возможны утечки.", file=sys.stderr)
    print(f"Детали: {e}", file=sys.stderr)
    libc = None

def _free_c_ptr(c_ptr):
    if libc and c_ptr:
        libc.free(c_ptr)

def fetch_data_from_c(branch_name: str) -> str | None:
    sys.stderr.write(f"Запрос пакетов для ветки '{branch_name}'...\n")
    c_result_ptr = librdb.fetch_package_list(branch_name.encode('utf-8'))
    
    if not c_result_ptr:
        sys.stderr.write(f"Ошибка: fetch_package_list вернула пустой указатель для '{branch_name}'.\n")
        return None
    
    try:

        json_bytes = ctypes.string_at(c_result_ptr) 
        json_data = json_bytes.decode('utf-8')
        return json_data
    except UnicodeDecodeError as e:
        sys.stderr.write(f"Ошибка: Не удалось декодировать ответ от C-библиотеки для '{branch_name}'.\n")
        sys.stderr.write(f"Детали: {e}\n")
        return None
    finally:
        _free_c_ptr(c_result_ptr) 

def compare_data_from_c(branch1_json: str, branch2_json: str) -> str | None:
    sys.stderr.write(f"Выполнение сравнения пакетов '{args.branch1}' и '{args.branch2}'...\n")
    c_result_ptr = librdb.compare_packages(branch1_json.encode('utf-8'), branch2_json.encode('utf-8'))

    if not c_result_ptr:
        sys.stderr.write("Ошибка: compare_packages вернула пустой указатель. Возможно, входные данные некорректны.\n")
        return None

    try:
        
        json_bytes = ctypes.string_at(c_result_ptr)
        json_data = json_bytes.decode('utf-8')
        return json_data
    except UnicodeDecodeError as e:
        sys.stderr.write(f"Ошибка: Не удалось декодировать ответ от C-библиотеки при сравнении.\n")
        sys.stderr.write(f"Детали: {e}\n")
        return None
    finally:
        _free_c_ptr(c_result_ptr) 

def print_comparison_results(results_json: dict, categories: list[str]):
    if not results_json or "architectures" not in results_json:
        sys.stderr.write("Ошибка: Некорректный формат JSON-результата сравнения.\n")
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
                    else:
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
    prog="rdb_compare",
    description="Инструмент для сравнения списков пакетов ALT Linux между ветками.",
    formatter_class=argparse.RawTextHelpFormatter,
    epilog="""Примеры использования:
  rdb_compare sisyphus p10
  rdb_compare p10 p9 -c branch1_only
  rdb_compare -s sisyphus
  rdb_compare -j p9 p10
  rdb_compare -t sisyphus p10 -c branch1_newer
"""
)
parser.add_argument(
    "branch1",
    nargs="?",
    default="sisyphus",
    help="Имя первой ветки (по умолчанию: sisyphus)."
)
parser.add_argument(
    "branch2",
    nargs="?",
    default="p10",
    help="Имя второй ветки (по умолчанию: p10). Используется только при сравнении."
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
        "  all            - все категории (по умолчанию)."
    )
)
parser.add_argument(
    "-j", "--json",
    action="store_true",
    help="Вывести необработанный JSON-ответ (как из C++ библиотеки)."
)
parser.add_argument(
    "-t", "--tree",
    action="store_true",
    help=(
        "Вывести результаты сравнения в человекочитаемом, "
        "иерархическом (древовидном) формате (по умолчанию, если -j не указан)."
    )
)
parser.add_argument(
    "-s", "--show-branch-json",
    action="store_true",
    help="Вывести необработанный JSON-список пакетов для BANCH1 (игнорирует BANCH2 и сравнение).",
)
parser.add_argument(
    "-v", "--version",
    action="version",
    version=f"%(prog)s {__version__}",
    help="Показать версию утилиты."
)

args = parser.parse_args()

# --- Основная логика скрипта ---

if args.show_branch_json:
    sys.stderr.write(f"Вывод JSON-списка пакетов для ветки '{args.branch1}'...\n")
    branch_data = fetch_data_from_c(args.branch1)
    if branch_data:
        try:
            parsed_json = json.loads(branch_data)
            print(json.dumps(parsed_json, indent=2, ensure_ascii=False))
        except json.JSONDecodeError as e:
            sys.stderr.write(f"Ошибка: Не удалось разобрать JSON-ответ для ветки '{args.branch1}'.\n")
            sys.stderr.write(f"Детали: {e}\n")
            sys.stderr.write(f"Необработанный JSON (частично): {branch_data[:500]}...\n")
            sys.exit(1)
    else:
        sys.exit(1)
else:
    branch1_data_json_str = fetch_data_from_c(args.branch1)
    if branch1_data_json_str is None:
        sys.exit(1)

    branch2_data_json_str = fetch_data_from_c(args.branch2)
    if branch2_data_json_str is None:
        sys.exit(1)

    comparison_json_str = compare_data_from_c(branch1_data_json_str, branch2_data_json_str)
    if comparison_json_str is None:
        sys.exit(1)

    if args.json:
        print(comparison_json_str)
    elif args.tree or (not args.json and not args.tree):
        try:
            comparison_results = json.loads(comparison_json_str)
            if args.category == "all":
                selected_categories = ["branch1_only", "branch2_only", "branch1_newer"]
            else:
                selected_categories = [args.category]
                
            print_comparison_results(comparison_results, selected_categories)

        except json.JSONDecodeError as e:
            sys.stderr.write(f"Ошибка: Не удалось разобрать JSON-ответ сравнения.\n")
            sys.stderr.write(f"Детали: {e}\n")
            sys.stderr.write(f"Необработанный JSON (частично): {comparison_json_str[:500]}...\n")
            sys.exit(1)

sys.exit(0)
