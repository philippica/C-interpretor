#ifndef STMT
#define STMT




extern Expr statement(int &cur,int continueFlag =0 , int breakFlag = 0);



Expr block(int &cur,int continueFlag = 0 , int breakFlag = 0)
{
	int read = cur;
	Expr ret;
	getToken(read, "{");
	while(1)
	{
		if(getToken(read, "}"))break;
		else
		{
			statement(read,continueFlag,breakFlag);
		}
	}
	cur = read;
	return ret;
}






extern Expr statement(int &cur,int continueFlag , int breakFlag)
{
	int read = cur;
	Expr ret;
	if(tokenStream[cur].identify == "{")
	{
		block(read);
	}
	else if(getToken(read,"if"))
	{
		getToken(read,"(");
		Expr exp = expression(read);
		getToken(read,")");
		Instru.insertInstruction("TEST " + exp.adress + " " + exp.adress);
		Instru.insertInstruction("JNE FLAG" + intToStr(flagIdx));
		int f1 = flagIdx++;
		statement(read);
		if(getToken(read, "else"))
		{
			Instru.insertInstruction("JMP FLAG" + intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG" + intToStr(f1));
			statement(read,continueFlag,breakFlag);
			Instru.insertInstruction("FLAG" + intToStr(f2));
		}
		else
		{
			Instru.insertInstruction("FLAG" + intToStr(f1));
		}
	}
	else if(getToken(read,"for"))
	{
		getToken(read,"(");
		Expr exp1 = expression(read);
		getToken(read,";");
		Instru.virtualOn();
		Expr exp2 = expression(read);
		vector <instruction> t1 = Instru.virtualOff();
		getToken(read,";");
		Instru.virtualOn();
		Expr exp3 = expression(read);
		vector <instruction> t2 = Instru.virtualOff();
		getToken(read,")");
		Instru.insertInstruction("JMP FLAG" + intToStr(flagIdx));
		int f1 = flagIdx++;
        Instru.insertInstruction("FLAG" + intToStr(flagIdx));
        int f2 = flagIdx++;
		int fbreak = flagIdx++;
		int fcontinue = flagIdx++;
		statement(read, fcontinue, fbreak);

		Instru.insertInstruction("FLAG" + intToStr(fcontinue));
		Instru.insertInstructions(t2);
		Instru.insertInstruction("FLAG" + intToStr(f1));
		Instru.insertInstructions(t1);
		if(exp2.adress!="")
		{
            Instru.insertInstruction("TEST " + exp2.adress + " " + exp2.adress);
            Instru.insertInstruction("JNE FLAG" + intToStr(f2));
        }
        else
        {
            Instru.insertInstruction("JMP FLAG" + intToStr(f2));
        }
		Instru.insertInstruction("FLAG" + intToStr(fbreak));
	}
	else if(getToken(read,"while"))
	{
		getToken(read,"(");
		Instru.virtualOn();
		Expr exp2 = expression(read);
		vector <instruction> t1 = Instru.virtualOff();
		getToken(read,")");
		Instru.insertInstruction("JMP FLAG" + intToStr(flagIdx));
		int f1 = flagIdx++;
        Instru.insertInstruction("FLAG" + intToStr(flagIdx));
        int f2 = flagIdx++;
		statement(read);
		Instru.insertInstruction("FLAG" + intToStr(f1));
		Instru.insertInstructions(t1);
		Instru.insertInstruction("TEST " + exp2.adress + " " + exp2.adress);
		Instru.insertInstruction("JNE FLAG" + intToStr(f2));
	}
	else if(getToken(read,"continue"))
	{
		getToken(read,";");
		Instru.insertInstruction("JMP FLAG" + intToStr(continueFlag));
	}
	else if(getToken(read,"break"))
	{
		getToken(read,";");
		Instru.insertInstruction("JMP FLAG" + intToStr(breakFlag));
	}
	else
	{
        declaration(read);
		//expressionStatement(read);
	}
	cur = read;
	return ret;
}




#endif
