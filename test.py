#!/usr/bin/env python3
import ctypes
import json
import sys

librdb = ctypes.CDLL("./librdbcompare.so")

librdb.fetch_package_list.restype = ctypes.c_char_p
librdb.fetch_package_list.argtypes = [ctypes.c_char_p]

branch = sys.argv[1] if len(sys.argv) > 1 else "sisyphus"
result = librdb.fetch_package_list(branch.encode('utf-8'))
if not result:
    print(f"Ошибка: Не удалось получить данные для ветки {branch}", file=sys.stderr)
    sys.exit(1)

print(json.loads(result.decode('utf-8')))

ctypes.c_free(result)