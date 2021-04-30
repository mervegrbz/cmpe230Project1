# mylang2ir:			main.o evaluation.o functions.o outputService.o assignments.o
# 					g++ main.o evaluation.o functions.o outputService.o assignments.o -o mylang2ir
# 					echo "done"

# main.o: 			main.cpp *.h
# 					g++ -c main.cpp *.h

# evaluation.o:		evaluation.cpp *.h
# 					g++ -c evaluation.cpp *.h

# functions.o:		functions.cpp *.h
# 					g++ -c functions.cpp *.h

# assignments.o:		assignments.cpp *.h
# 					g++ -c assignments.cpp *.h

# outputService.o:	outputService.cpp *.h
# 					g++ -c outputService.cpp *.h

mylang2ir: 		main.cpp evaluation.* functions.* outputService.* assignments.*
				g++ *.cpp *.h -o mylang2ir
				echo "done"

