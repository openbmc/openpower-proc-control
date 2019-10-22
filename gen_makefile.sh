#!/bin/bash

files=()
dir=$1               #Base directory
types=($2)           #List of types (convert to array)
isEKBRequired=$3     #To check wthether pHAL based host boot is required
target_chip=$4       #To get target HWP header file from ekb if it is pHAL

if [ $isEKBRequired == "yes" ]
then
    echo
    echo "openpower_proc_control_CXXFLAGS += -DFAPI2_NO_FFDC"
    echo "openpower_proc_control_CXXFLAGS += -I\$(INCDIR)/hwpf/fapi2/include"
    echo "openpower_proc_control_CXXFLAGS += -I\$(INCDIR)/hwpf/fapi2/include/attributes"
    echo "openpower_proc_control_CXXFLAGS += -I\$(INCDIR)/hwpf/fapi2/include/error_info"
    echo "openpower_proc_control_CXXFLAGS += -I\$(INCDIR)/hwpf/fapi2/include/plat"
    echo "openpower_proc_control_CXXFLAGS += -I\$(INCDIR)/ekb/hwpf/fapi2/include"
    echo "openpower_proc_control_CXXFLAGS += -I\$(INCDIR)/ekb/chips/$target_chip/procedures/hwp/perv/"
    echo
fi

echo "openpower_procedures_cpp_files = \\"
for ((i=0; i<${#types[@]}; ++i));
do
    type=${types[$i]}
    type=${type// /} #remove spaces
    if [ \( $isEKBRequired == "yes" \) -a \( $type == $target_chip \) ]
    then
        type="ekb/"$target_chip
    fi

    for file in $(ls $dir/procedures/$type/*.cpp);
    do
        files+=($file)
    done
done

for file in ${files[@]};
do
    echo "	$file \\"
done
echo

cat << MAKEFILE
openpower_procedures.cpp: \$(openpower_procedures_cpp_files)
	cat \$^ > \$@

MAKEFILE
