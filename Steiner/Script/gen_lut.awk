# AWK script per MAWK/Windows per generare STSC_LUT.h
BEGIN {
    FS=",";
    OFS="";
    max_lut = 0;
    print "#ifndef _STSC_LUT_H_\n#define _STSC_LUT_H_\n";
    print "#include <stdint.h>\n";
}

# Legge i file CSV e popola LUT e terne
to_process == 0 {
    while (("ls *.csv | sort" | getline file) > 0) {
        files[to_process++] = file;
    }
    close("ls *.csv | sort");
}

FILENAME != last_file {
    last_file = FILENAME;
    getline;
    split($0, header, FS);
    v = header[4];
    td = header[5];
    lut_entries[max_lut] = v " " td;
    max_lut++;
}

NR > 1 {
    terne[v][NR-1] = $1 " " $2 " " $3;
}

END {
    print "#define MAX_LUT " max_lut;
    print "const uint32_t LUT[MAX_LUT][2] = {";
    for (i = 0; i < max_lut; i++) {
        print "   {" lut_entries[i] "},";
    }
    print "};\n";

    print "typedef struct { uint32_t a, b, c; } Terna_t;\n";
    print "const Terna_t TERNE_PELTESOHN[MAX_LUT][MAX_DIFF] = {";
    
    for (i = 0; i < max_lut; i++) {
        split(lut_entries[i], vt, " ");
        v = vt[1];
        print "   // v = " v;
        print "   {";
        for (j in terne[v]) {
            print "      {" terne[v][j] "},";
        }
        print "   },";
    }
    print "};\n\n#endif";
}
