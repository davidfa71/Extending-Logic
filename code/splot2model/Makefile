splot2model : splot2model.l
	/usr/bin/flex --c++ -L splot2model.l && g++ -Wno-deprecated-register -I../tools/src -std=c++11 -O2 -o ../bin/splot2model lex.yy.cc ../tools/src/synExp.cpp ../tools/src/symbolTable.cpp ../tools/src/eval.cpp ../tools/src/varInfo.cpp ../tools/src/configInfo.cpp ../tools/src/choiceInfo.cpp ../tools/src/mytime.cpp
	@rm -f lex.yy.cc 
clean : 
	@rm -f  ../bin/splot2model lex.yy.cc 
