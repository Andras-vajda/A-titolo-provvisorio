import os
import csv
from collections import defaultdict

def parse_csv_files(folder_path):
    """Legge tutti i file CSV nella cartella, ordinandoli lessicograficamente."""
    csv_files = sorted(f for f in os.listdir(folder_path) if f.endswith('.csv'))
    lut = []  # Lista per la LUT (v, td)
    terne = defaultdict(list)  # Dizionario per le terne
    
    for file_name in csv_files:
        file_path = os.path.join(folder_path, file_name)
        with open(file_path, newline='') as csvfile:
            reader = csv.reader(csvfile)
            header = next(reader)  # f"a,b,c,{v},{td}"
            v, td = int(header[3]), int(header[4])
            lut.append((v, td))
            for row in reader:
                a, b, c = map(int, row[:3])
                terne[v].append((a, b, c))
    
    return lut, terne

def generate_c_header(lut, terne, output_file="STSC_LUT.h"):
    """Genera l'header C contenente la LUT e la matrice di terne."""
    with open(output_file, "w") as f:
        f.write("#ifndef _STSC_LUT_H_\n#define _STSC_LUT_H_\n\n")
        f.write("#include <stdint.h>\n\n")
        
        # LUT
        f.write(f"#define MAX_LUT {len(lut)}\n")
        f.write("const uint32_t LUT[MAX_LUT][2] = {\n")
        f.write(",\n".join(f"   {{{v}, {td}}}" for v, td in lut))
        f.write("\n};\n\n")
        
        # Struttura Terna_t
        f.write("typedef struct { uint32_t a, b, c; } Terna_t;\n\n")
        
        # Terne differenza
        f.write("const Terna_t TERNE_PELTESOHN[MAX_LUT][MAX_DIFF] = {\n")
        for v, _ in lut:
            f.write(f"   // v = {v}\n   {{")
            f.write(", ".join(f"{{{a}, {b}, {c}}}" for a, b, c in terne[v]))
            f.write("}},\n")
        f.write("};\n\n#endif")

if __name__ == "__main__":
    folder_path = "./data"  # Modifica con il percorso corretto
    lut, terne = parse_csv_files(folder_path)
    generate_c_header(lut, terne)
    print("STSC_LUT.h generato con successo!")
