#!/usr/bin/env tclsh

proc parse_csv_files {folder_path} {
    set files [lsort [glob -nocomplain -directory $folder_path *.csv]]
    set lut {}
    array set terne {}
    
    foreach file $files {
        set fp [open "$folder_path/$file" r]
        set header [gets $fp]
        set fields [split $header ","]
        set v [lindex $fields 3]
        set td [lindex $fields 4]
        lappend lut "$v $td"
        
        while {[gets $fp line] >= 0} {
            set values [split $line ","]
            lappend terne($v) "[lindex $values 0] [lindex $values 1] [lindex $values 2]"
        }
        close $fp
    }
    return [list $lut [array get terne]]
}

proc generate_c_header {lut terne output_file} {
    set fp [open $output_file w]
    puts $fp "#ifndef _STSC_LUT_H_\n#define _STSC_LUT_H_\n"
    puts $fp "#include <stdint.h>\n"
    
    puts $fp "#define MAX_LUT [llength $lut]"
    puts $fp "const uint32_t LUT[MAX_LUT][2] = {"
    foreach pair $lut { puts $fp "   {$pair}," }
    puts $fp "};\n"

    puts $fp "typedef struct { uint32_t a, b, c; } Terna_t;\n"
    puts $fp "const Terna_t TERNE_PELTESOHN[MAX_LUT][MAX_DIFF] = {"
    
    foreach {v terne_list} $terne {
        puts $fp "   // v = $v"
        puts $fp "   {"
        foreach terna $terne_list { puts $fp "      {$terna}," }
        puts $fp "   },"
    }
    puts $fp "};\n"
    puts $fp "#endif"
    close $fp
}

set folder_path "./csv_input"
set result [parse_csv_files $folder_path]
set lut [lindex $result 0]
set terne [lindex $result 1]
generate_c_header $lut $terne "STSC_LUT.h"
puts "STSC_LUT.h generato con successo!"
