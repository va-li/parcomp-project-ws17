add_executable(data_generator data_generator/data_generator.c)

add_custom_command(TARGET data_generator COMMAND mkdir ../exes/ -p && cp -f data_generator ../exes/data_generator && echo "Copied the new executable")