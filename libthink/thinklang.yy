/* $Id: thinklang.yy,v 1.47 2003/11/04 00:52:49 misha Exp $ */

%{
#include "think.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thArg.h"
#include "thPlugin.h"
#include "thNode.h"
#include "thMod.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thMidiNote.h"
#include "thMidiChan.h"
#include "thSynth.h"

#include "yygrammar.h"
#include "parser.h"

thMod *parsemod;
thNode *parsenode;
thSynth Synth;

// XXX - REIMPLEMENT GLOBAL STUFFS
// modnode *parsemod = NULL;
// static linked_list *targs = NULL;

void yyerror (const char *str)
{
	fprintf(stderr, "line %d: error: %s\n", linenum, str);
}

extern "C" int yywrap(void)
{
	return 1;
}
%}

%token NODE IO NAME
%token WORD 
%token FLOAT NUMBER
%token ENDSTATE ASSIGN LCBRACK RCBRACK
%token INTO
%token MODSEP
%token ADD SUB MUL DIV MOD CPAREN OPAREN NIL
%token DOLLAR
%token STRING

%%

statements:
|
statements statement ENDSTATE
;

statement:
nodes
|
nodes nodes
{
	yyerror("missing semicolon after node\n");
	YYERROR;
}
|
ionode
|
nameset
|
expression
{
	printf("%f\n", $1.floatval);
}
;

expression:
simple_expression
;

simple_expression:
unsigned_simple_expression
{
	$$.floatval = $1.floatval;
}
|
SUB unsigned_simple_expression
{
	$$.floatval = $2.floatval*-1;
}
;

unsigned_simple_expression:
term
{
	$$.floatval = $1.floatval;
}
|
term ADD unsigned_simple_expression
{
	$$.floatval = $1.floatval + $3.floatval;
}
|
term SUB unsigned_simple_expression
{
	$$.floatval = $1.floatval - $3.floatval;
}
;

term:
factor
|
factor MUL term
{
	$$.floatval = $1.floatval * $3.floatval;
}
|
factor DIV term
{
	$$.floatval = $1.floatval / $3.floatval;
}
|
factor MOD term
{
	$$.floatval = ((int)$1.floatval) % ((int)$3.floatval);
}
;

factor:
OPAREN expression CPAREN
{
	$$.floatval = $2.floatval;
}
|
unsigned_constant
{
	$$.floatval = $1.floatval;
}
;

unsigned_constant:
NUMBER
{
	$$.floatval = $1.floatval;
}
|
NIL
;

nodes:
NODE WORD plugname LCBRACK assignments RCBRACK
{
#ifdef USE_DEBUG
	printf("node %s defined using plugin %s\n", $2.str, $3.str);

	printf("Checking if plugin %s is loaded...\n", $3.str);
#endif
	if(!((thPluginManager *)Synth.GetPluginManager())->GetPlugin($3.str))
		{
			if (((thPluginManager *)Synth.GetPluginManager())->LoadPlugin($3.str) == 1) { /* FAILED */
				printf ("Error loading required %s, aborting.\n", $3.str);
				exit(1);
			}
		}
	parsenode->SetPlugin(((thPluginManager *)Synth.GetPluginManager())->GetPlugin($3.str));

	delete[] $3.str;

	parsenode->SetName($2.str);
	parsemod->NewNode(parsenode);
	parsenode = new thNode("newnode", NULL);		/* add name, plugin */

	free($2.str);
}
|
NODE WORD LCBRACK assignments RCBRACK
{
#ifdef USE_DEBUG
	printf("node %s defined with no plugin\n", $2.str);
#endif

	parsenode->SetName($2.str);
	parsenode->SetPlugin(NULL);
	parsemod->NewNode(parsenode);
	parsenode = new thNode("newnode", NULL);

	free($2.str);
}
;

ionode:
IO WORD
{
	printf("IO node defined as %s\n", $2.str);
	parsemod->SetIONode($2.str);
}
;

nameset:
NAME STRING
{
	parsemod->SetName($2.str);
	free($2.str);
	printf("DSP Name: %s\n", parsemod->GetName().c_str());
}
;

assignments:
|
assignments assignment ENDSTATE
;

assignment:
WORD ASSIGN expression
{
//	XXX - MORE FIXING
//	modify_num(&targs, (char *)$1.str, $3.floatval);

	float *copy = new float[1];
	*copy = $3.floatval;
	parsenode->SetArg($1.str, copy, 1);
	free($1.str);
}
|
WORD ASSIGN fstr
{
	char *node, *arg, *p;
	int argsize, nodesize;
	
	/* Make $3.str ("node/arg" format) into the above vars */
	p = strchr($3.str, '/');
	p++;
	
	argsize = strlen(p);
	nodesize = strlen($3.str)-argsize-1;
	
	node = new char[nodesize+1];
	memcpy(node, $3.str, nodesize);
	node[nodesize] = 0;

	arg = new char[argsize+1];
	memcpy(arg, p, argsize);
	arg[argsize] = 0;
	parsenode->SetArg($1.str, node, arg);

	delete[] node;
	delete[] arg;
	delete[] $3.str;
	free($1.str);
}
;

plugname:
WORD MODSEP WORD
{
	$$.str = new char[strlen($1.str) + strlen($3.str) + 2];
	sprintf((char *)$$.str, "%s/%s", $1.str, $3.str);
	free($1.str);
	free($3.str);
}
|
WORD MODSEP plugname
{
	$$.str = new char[strlen($1.str) + strlen($3.str) + 2];
	sprintf((char *)$$.str, "%s/%s", $1.str, $3.str);
	delete $3.str;
	free($1.str);
}
;

fstr:		/* a node name and an fstring */
WORD INTO WORD
{
	/* we must allocate two extra bytes; one for the '/' and one for the null
       terminator */
	$$.str = new char[strlen($1.str) + strlen($3.str) + 2];
	sprintf((char *)$$.str, "%s/%s", $1.str, $3.str);
	free($1.str);
	free($3.str);
}
;
%%
