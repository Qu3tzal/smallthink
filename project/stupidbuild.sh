# Clear.
rm ./../linux/bin/*

# Build.
g++ -Wall -Wfatal-errors -Werror -Wextra -Wold-style-cast -Woverloaded-virtual -Wfloat-equal -Wwrite-strings -Wpointer-arith -Wcast-qual -Wcast-align -Wconversion -Wshadow -Wredundant-decls -Wdouble-promotion -Winit-self -Wswitch-default -Wswitch-enum -Wundef -Wlogical-op -Winline ./../src/* -o ./../linux/bin/smallthink

# Promote.
chmod +x ./../bin/linux/smallthink

# Run
./../linux/bin/smallthink
