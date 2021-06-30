from struct import unpack
from binascii import unhexlify
import sys
import os

REAL_OUT     = sys.argv[1]
EXPECTED_OUT = sys.argv[2]
SHOULD_HEXA_PRINT = (len(sys.argv) > 3 and sys.argv[3] == "-h")

HEX_FOLDER_NAME = "hex_files"
DEBUG_FOLDER_NAME = "debug_files"

BAR = "=" * 50

def hex_to_int(hex):
    amt_bytes = len(hex) // 2

    val = 0
    pot = 1

    for i in range(amt_bytes):
        val += int(hex[2*i + 1], 16) * pot
        pot *= 16
        val += int(hex[2*i], 16) * pot
        pot *= 16

    return val

def line_read(line, size):
    part = line[:size]

    return part, line[size:]

def write_a_node(file, content_line):
    strlen = len(content_line.strip()) // 2
    if strlen != 77:
        file.write(f"\tInvalid line: index_data_size == {strlen} != 77, check the .hex file in {HEX_FOLDER_NAME}\n")
        return

    is_leaf_hex, content_line = line_read(content_line, 2)
    is_leaf = "eh" if is_leaf_hex == '31' else "num eh"
    if (SHOULD_HEXA_PRINT): file.write(f"\tis_leaf: {is_leaf} [0x{is_leaf_hex}] ")
    else: file.write(f"\tis_leaf: {is_leaf} ")
    
    amt_keys_hex, content_line = line_read(content_line, 8)
    amt_key = hex_to_int(amt_keys_hex)
    if (SHOULD_HEXA_PRINT): file.write(f"| amt_keys: {amt_key} [0x{amt_keys_hex}] |")
    else: file.write(f"| amt_keys: {amt_key:<10d} |")

    node_rrn_hex, content_line = line_read(content_line, 8)
    node_rrn = hex_to_int(node_rrn_hex)
    if (SHOULD_HEXA_PRINT): file.write(f"| node_rrn: {node_rrn} [0x{node_rrn_hex}]\n\n")
    else: file.write(f" node_rrn: {node_rrn:<10d}\n\n")

    p_1_hex, content_line = line_read(content_line, 8)
    p_1 = hex_to_int(p_1_hex) if p_1_hex.count('f') != 8 else "NULL (-1)"
    if (SHOULD_HEXA_PRINT): file.write(f"\t----> p_1: {p_1} [{p_1_hex}] \n\n")   
    else: file.write(f"\t----> p_1: {p_1:<10}\n\n")   

    for i in range(4):
        r_key_hex, content_line = line_read(content_line, 8)
        r_key = hex_to_int(r_key_hex) if r_key_hex.count('f') != 8 else "NULL (-1)"
        if (SHOULD_HEXA_PRINT): file.write(f"\treg_{i+2} < key: {r_key} [0x{r_key_hex}]")
        else: file.write(f"\treg_{i+2} < key: {r_key:<10}")

        r_p_r_hex, content_line = line_read(content_line, 16)
        r_p_r = hex_to_int(r_p_r_hex) if r_p_r_hex.count('f') != 16 else "NULL (-1)"
        if (SHOULD_HEXA_PRINT): file.write(f", p_r: {r_p_r} [0x{r_p_r}] >\n")
        else: file.write(f", p_r: {r_p_r:<20} >\n")

        p_hex, content_line = line_read(content_line, 8)
        p = hex_to_int(p_hex) if p_hex.count('f') != 8 else "NULL (-1)"
        if (SHOULD_HEXA_PRINT): file.write(f"\t----> p_{i+2}: {p} [0x{p_hex}]\n\n")
        else: file.write(f"\t----> p_{i+2}: {p:<10}\n\n")

def hex_beautify(filename):
    debug_filename = f"{DEBUG_FOLDER_NAME}/{filename.split('.')[0].split('/')[-1]}.debug"
    beautiful_file = open(debug_filename, "w")
    hex_file       = open(filename, "r")

    header_title = f"{BAR} HEADER {BAR}\n"
    beautiful_file.write(header_title)

    status_hex = hex_file.read(2)
    status     = "consistente" if status_hex == '31' else "inconsistente"
    if (SHOULD_HEXA_PRINT): beautiful_file.write(f"status: {status} [0x{status_hex}] ")
    else: beautiful_file.write(f"status: {status} ")

    root_rrn_hex = hex_file.read(8)
    root_rrn     = hex_to_int(root_rrn_hex);
    if (SHOULD_HEXA_PRINT): beautiful_file.write(f"| root_rrn: {root_rrn} [0x{root_rrn_hex}] |")
    else: beautiful_file.write(f"| root_rrn: {root_rrn:<10d} |")

    nxt_node_rrn_hex = hex_file.read(8)
    nxt_node_rrn     = hex_to_int(nxt_node_rrn_hex);
    if (SHOULD_HEXA_PRINT): beautiful_file.write(f" nxt_node_rrn: {nxt_node_rrn} [0x{nxt_node_rrn_hex}]\n")
    else: beautiful_file.write(f" nxt_node_rrn: {nxt_node_rrn:<10d}\n")
    hex_file.readline()

    node_idx = 0
    data_title = f"{BAR}= DATA ={BAR}\n"
    beautiful_file.write(data_title)
    for line in hex_file:
        beautiful_file.write(f"node {node_idx:02d}:\n")
        write_a_node(beautiful_file, line)
        node_idx += 1

    beautiful_file.close()
    hex_file.close()

if (os.path.getsize(REAL_OUT) % 77 != 0):
    print(f"O arquivo {REAL_OUT} não possui um tamanho multiplo de 77 ...")
if (os.path.getsize(EXPECTED_OUT) % 77 != 0):
    print(f"O arquivo {EXPECTED_OUT} não possui um tamanho multiplo de 77 ...")
print()

os.system(f"mkdir {HEX_FOLDER_NAME} -p")
os.system(f"mkdir {DEBUG_FOLDER_NAME} -p")

cmd = "hexdump -e '77/1 \"%02x\" \"\n\"' "
os.system(f"{cmd} {EXPECTED_OUT} > {HEX_FOLDER_NAME}/expected.hex")
os.system(f"{cmd} {REAL_OUT} > {HEX_FOLDER_NAME}/my.hex")

hex_beautify(f"{HEX_FOLDER_NAME}/expected.hex")
hex_beautify(f"{HEX_FOLDER_NAME}/my.hex")