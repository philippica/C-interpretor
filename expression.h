#ifndef EXPR
#define EXPR



struct Expr
{
	bool error;
	bool isNum;
	string adress;
	int value;
	string errorInformation;
	Expr():error(true),isNum(true),value(0){};
};

string intToStr(int x)
{
	string ret = "";
	char temp[100];
	int h=0;
	while (x)
	{
		temp[++h] = (char)(x % 10 + '0');
		x/=10;
	}
	for(int i=h;i>=1;i--)ret+=temp[i];
	if(ret == "")ret = "0";
	return ret;
}


bool getToken(int &cur, string str)
{
	if(tokenStream[cur].identify == str)
	{
		cur++;
		return true;
	}
	return false;
}

bool variable(int &cur)
{
	if(tokenStream[cur].symble == "variable")return true;else return false;
}


Expr expression(int &cur);
Expr assignmentExpression(int &cur);
int getNum(int cur)
{
	int len = tokenStream[cur].identify.size();
	int ret = 0;
	for(int i = 0; i < len; i++)
	{
		ret = ret * 10 + tokenStream[cur].identify[i] - '0';
	}
	return ret;
}

int pp=0,flagIdx = 0;
Expr primaryExpr(int &cur)
{
	Expr ret;
	if(getToken(cur, "int") || getToken(cur, "float") || getToken(cur, "string"))
	{
		cur++;
		return ret;
	}
	if(tokenStream[cur].symble == "integer")
	{
		ret.value = getNum(cur);
		Instru.insertInstruction("MOV @"+intToStr(pp)+" "+intToStr(ret.value));
		ret.adress = "@" + intToStr(pp);
		pp++;
		cur++;
		return ret;
	}
	if(variable(cur))
	{
		cur++;
		ret.adress = tokenStream[cur-1].identify;
		return ret;
	}
	int read = cur;
	if(getToken(read, "("))
	{
		Expr temp = expression(read);
		getToken(read, ")");
		ret = temp;
	}
	cur = read;
	return ret;
}


Expr argumentExpressionList(int &cur)
{
	int read = cur;
	Expr ret = assignmentExpression(read);
	while(true)
	{
		if(getToken(read, ","))
		{
			Expr temp = assignmentExpression(read);
			ret = temp;
		}
	}
	cur = read;
	return ret;
}
Expr postfixExpression(int &cur)
{
	int read = cur;
	Expr ret = primaryExpr(read);
	while(true)
	{
		if(getToken(read, "["))
		{
			expression(read);
			getToken(read, "]");
		}
		else if(getToken(read, "."))
		{
			variable(read);
		}
		else if(getToken(read, "->"))
		{
			variable(read);
		}
		else if(getToken(read, "++"))
		{
			Instru.insertInstruction("INC "+ret.adress);
			ret.value++;
		}
		else if(getToken(read, "--"))
		{
			Instru.insertInstruction("DEC "+ret.adress);
			ret.value--;
		}
		else if(getToken(read, "("))
		{
			if(!getToken(read, ")"))
			{
				Expr temp = argumentExpressionList(read);
				ret = temp;
				getToken(read, ")");
			}
		}
		else break;
	}
	cur = read;
	return ret;
}

bool typeName(int &cur)
{
	if(tokenStream[cur].identify == "int")
	{
		return true;
	}
	return false;
}

Expr unaryExpression(int &cur);

Expr castExpression(int &cur)
{
	int read = cur;
	Expr ret;
	if(tokenStream[cur].identify == "(" && typeName(read))
	{
		getToken(read, "(");
		typeName(read);
		getToken(read, ")");
		castExpression(read);
	}
	else
	{
		Expr temp = unaryExpression(read);
		ret = temp;
	}
	cur = read;
	return ret;
}

Expr unaryExpression(int &cur)
{
	int read = cur;
	Expr ret;
	if(getToken(read, "++"))
	{
		Expr temp = unaryExpression(read);
		temp.value++;
		Instru.insertInstruction("INC "+temp.adress);
		ret = temp;
	}
	else if(getToken(read, "&"))
	{
		Expr temp = castExpression(read);
		ret = temp;
	}
	else if(getToken(read, "*"))
	{
		Expr temp = castExpression(read);
		ret = temp;
	}
	else if(getToken(read, "+"))
	{
		Expr temp = castExpression(read);
		ret = temp;
	}
	else if(getToken(read, "-"))
	{
		Expr temp = castExpression(read);
		Instru.insertInstruction("MUL "+temp.adress+" -1");
		temp.value = - temp.value;
		ret = temp;
	}
	else if(getToken(read, "~"))
	{
		Expr temp = castExpression(read);
		ret = temp;
	}
	else if(getToken(read, "!"))
	{
		Expr temp = castExpression(read);
	}
	else if(getToken(read, "sizeof"))
	{
		if(getToken(read, "("))
		{
			typeName(read);
			getToken(read, ")");
		}
	}
	else
	{
		Expr temp = postfixExpression(read);
		ret = temp;
	}
	cur = read;
	return ret;
}


Expr multiplicativeExpression(int &cur)
{
	int read = cur;
	Expr ret = castExpression(read);
	while(true)
	{
		if(getToken(read, "*"))
		{
			Expr temp = castExpression(read);
			Instru.insertInstruction("MUL "+ret.adress+" "+temp.adress);
			ret.value *= temp.value;
		}
		else if(getToken(read, "/"))
		{
			Expr temp = castExpression(read);
			Instru.insertInstruction("DIV "+ret.adress+" "+temp.adress);
			ret.value /= temp.value;
		}
		else if(getToken(read, "%"))
		{
			Expr temp = castExpression(read);
			Instru.insertInstruction("MOD "+ret.adress+" "+temp.adress);
			ret.value %= temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}

Expr additiveExpression(int &cur)
{
	int read = cur;
	Expr ret = multiplicativeExpression(read);
	while(true)
	{
		if(getToken(read, "+"))
		{
			Expr temp = multiplicativeExpression(read);
			Instru.insertInstruction("ADD "+ret.adress+" "+temp.adress);
			//ret.adress = "@"+ intToStr(pp);
			ret.value += temp.value;
		}
		else if(getToken(read, "-"))
		{
			Expr temp = multiplicativeExpression(read);
			Instru.insertInstruction("SUB "+ret.adress+" "+temp.adress);
			ret.value -= temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}
Expr shiftExpression(int &cur)
{
	int read = cur;
	Expr ret = additiveExpression(read);
	while(true)
	{
		if(getToken(read, "<<"))
		{
			Expr temp = additiveExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("SHL "+ret.adress+" "+temp.adress);
			ret.value <<= temp.value;
		}
		else if(getToken(read, ">>"))
		{
			Expr temp = additiveExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("SHR "+ret.adress+" "+temp.adress);
			ret.value >>= temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}

Expr relationalExpression(int &cur)
{
	int read = cur;
	Expr ret = shiftExpression(read);
	while(true)
	{
		if(getToken(read, "<="))
		{
			Expr temp = shiftExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("CMP "+ret.adress+" "+temp.adress);
			Instru.insertInstruction("JG FLAG"+ intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+ intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG"+intToStr(f2));
			ret.value = (ret.value <= temp.value);
		}
		else if(getToken(read, ">="))
		{
			Expr temp = shiftExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("CMP "+ret.adress+" "+temp.adress);
			Instru.insertInstruction("JL FLAG"+ intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+ intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG"+intToStr(f2));
			ret.value = (ret.value >= temp.value);
		}
		else if(getToken(read, ">"))
		{
			Expr temp = shiftExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("CMP "+ret.adress+" "+temp.adress);
			Instru.insertInstruction("JLE FLAG"+ intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+ intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG"+intToStr(f2));
			ret.value = (ret.value > temp.value);
		}
		else if(getToken(read, "<"))
		{
			Expr temp = shiftExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("CMP "+ret.adress+" "+temp.adress);
			Instru.insertInstruction("JGE FLAG"+ intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+ intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG"+intToStr(f2));
			ret.value = (ret.value < temp.value);
		}
		else break;
	}
	cur = read;
	return ret;
}

Expr equalityExpression(int &cur)
{
	int read = cur;
	Expr ret = relationalExpression(read);
	while(true)
	{
		if(getToken(read, "=="))
		{
			Expr temp = relationalExpression(read);
            Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("CMP "+ret.adress+" "+temp.adress);
			Instru.insertInstruction("JNE FLAG"+ intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG"+intToStr(f2));
			ret.value = (ret.value == temp.value);
		}
		else if(getToken(read, "!="))
		{
			Expr temp = relationalExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("CMP "+ret.adress+" "+temp.adress);
			Instru.insertInstruction("JE FLAG"+ intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG"+intToStr(f2));
			ret.value = (ret.value != temp.value);
		}
		else break;
	}
	cur = read;
	return ret;
}

Expr andExpression(int &cur)
{
	int read = cur;
	Expr ret = equalityExpression(read);
	while(true)
	{
		if(getToken(read, "&"))
		{
			Expr temp = equalityExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("AND "+ret.adress+" "+temp.adress);
			ret.value &= temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}


Expr exclusiveOrExpression(int &cur)
{
	int read = cur;
	Expr ret = andExpression(read);
	while(true)
	{
		if(getToken(read, "^"))
		{
			Expr temp = andExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("XOR "+ret.adress+" "+temp.adress);
			ret.value ^= temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}

Expr OrExpression(int &cur)
{
	int read = cur;
	Expr ret = exclusiveOrExpression(read);
	while(true)
	{
		if(getToken(read, "|"))
		{
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Expr temp = exclusiveOrExpression(read);
			Instru.insertInstruction("OR "+ret.adress+" "+temp.adress);
			ret.value |= temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}

Expr logicAndExpression(int &cur)
{
	int read = cur;
	Expr ret = OrExpression(read);
	while(true)
	{
		if(getToken(read, "&&"))
		{
			Expr temp = OrExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("TEST "+temp.adress+" "+temp.adress);
			Instru.insertInstruction("JE FLAG"+intToStr(flagIdx));
			Instru.insertInstruction("TEST "+ret.adress+" "+ret.adress);
			Instru.insertInstruction("JE FLAG"+intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG" + intToStr(f2));
			ret.value = ret.value && temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}


Expr logicOrExpression(int &cur)
{
	int read = cur;
	Expr ret = logicAndExpression(read);
	while(true)
	{
		if(getToken(read, "||"))
		{
			Expr temp = logicAndExpression(read);
			Instru.insertInstruction("MOV @" + intToStr(pp) + " " + ret.adress);
            ret.adress = "@" + intToStr(pp++);
			Instru.insertInstruction("TEST "+temp.adress+" "+temp.adress);
			Instru.insertInstruction("JNE FLAG"+intToStr(flagIdx));
			Instru.insertInstruction("TEST "+ret.adress+" "+ret.adress);
			Instru.insertInstruction("JNE FLAG"+intToStr(flagIdx));
			int f1 = flagIdx++;
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP FLAG"+intToStr(flagIdx));
			int f2 = flagIdx++;
			Instru.insertInstruction("FLAG"+intToStr(f1));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("FLAG" + intToStr(f2));
			ret.value = ret.value || temp.value;
		}
		else break;
	}
	cur = read;
	return ret;
}



Expr conditionalExpression(int &cur)
{
	int read = cur;
	Expr ret = logicOrExpression(read);
	while(true)
	{
		if(getToken(read, "?"))
		{
			Instru.insertInstruction("TEST "+ret.adress+" "+ret.adress);
			Instru.insertInstruction("JE "+intToStr(Instru.curLabel() + 5));
			Instru.insertInstruction("TEST "+ret.adress+" "+ret.adress);
			Instru.insertInstruction("JE "+intToStr(Instru.curLabel() + 3));
			Instru.insertInstruction("MOV " + ret.adress + " 0x01");
			Instru.insertInstruction("JMP "+intToStr(Instru.curLabel() + 2));
			Instru.insertInstruction("MOV " + ret.adress + " 0x00");
			Instru.insertInstruction("NOP");
			Expr temp = expression(read);
			getToken(read, ":");
			Expr temp3 = conditionalExpression(read);
		}
		else break;
	}
	cur = read;
	return ret;
}



Expr assignmentExpression(int &cur)
{
	int recover;
	int read = recover = cur;
	int tempp = pp;
	instr u = Instru;
	Expr ret = unaryExpression(read);
	if(getToken(read, "="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("MOV " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "*="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("MUL " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "+="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("ADD " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "-="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("SUB " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "/="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("DIV " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "%="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("MOD " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "<<="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("SHL " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, ">>="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("SHR " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "&="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("AND " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "|="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("OR " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else if(getToken(read, "^="))
	{
		Expr temp = assignmentExpression(read);
		Instru.insertInstruction("XOR " + ret.adress + " " + temp.adress);
		ret = temp;
	}
	else
	{
		read = recover;
		Instru = u;
		pp = tempp;
		ret = conditionalExpression(read);
	}

	cur = read;
	return ret;
}


Expr expression(int &cur)
{
	int read = cur;
	Expr ret = assignmentExpression(read);
	while(true)
	{
		if(getToken(read, ","))
		{
			Expr temp = assignmentExpression(read);
			ret = temp;
		}
		else break;
	}
	if(read == cur)ret.error = false;
	cur = read;
	return ret;
}

Expr expressionStatement(int &cur)
{
	int read = cur;
	Expr ret = expression(read);
	if(!ret.error)
	{
		Expr temp = expression(read);
		ret = temp;
	}
	getToken(read,";");
	cur = read;
	return ret;
}


#endif
