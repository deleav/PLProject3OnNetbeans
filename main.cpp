# include <stdlib.h>
# include <iostream>
# include <string>
# include <sstream>
# include <vector>
# include <iomanip>

using namespace std;

static int uTestNum = -1;

// /////////////////////////////////////////////////////////////////////////////
//                               class & struct                               //
// /////////////////////////////////////////////////////////////////////////////

class Token {
public:
  string mToken;
  int mX;
  int mY;

  Token() {
    mX = -1;
    mY = -1;
  } // Token()

  Token( string str ) {
    mToken = str;
    mX = -1;
    mY = -1;
  } // Token()

  Token( string str, int x, int y ) {
    mToken = str;
    mX = x;
    mY = y;
  } // Token()
}; // class Token

class Index {
public:
  int mX;
  int mY;

  Index() {
    mX = -1;
    mY = -1;
  } // Index()

  Index( int x, int y ) {
    mX = x;
    mY = y;
  } // Index()
}; // class Index

class FunctionToken {
public:
  string mToken;
  vector<Token> mFunctionToken;
  int mParamsNum;

  FunctionToken( string name, vector<Token> functionToken ) {
    mToken = name;
    mFunctionToken = functionToken;
    mParamsNum = 0;
  } // FunctionToken()
}; // class FunctionToken

class Ident {
public:
  string mToken;
  string mType;
  int mX;
  int mY;
  int mArraySize;

  Ident() {
    mToken = "";
    mType = "";
    mX = 0;
    mY = 0;
    mArraySize = 0;
  } // Ident()

  Ident( Token token, string type ) {
    mToken = token.mToken;
    mX = token.mX;
    mY = token.mY;
    mType = type;
  } // Ident()

  Ident( Token token, string type, int arraySize ) {
    mToken = token.mToken;
    mX = token.mX;
    mY = token.mY;
    mType = type;
    mArraySize = arraySize;
  } // Ident()
}; // class Ident

class Table {
public:
  vector<string> mTable1; // declare
  vector<string> mTable2; // operator
  vector<string> mTable3;
  // void      -> ( params )
  // if, while -> ( boolExp )
  // else, do  -> { stmt }

  Table() {
    mTable1 = GetTable1();
    mTable2 = GetTable2();
    mTable3 = GetTable3();
  } // Table()

private:
  vector<string> GetTable1() {
    string strArray[] = { "int", "float", "char", "bool", "string" };
    vector<string> table1;
    for ( int i = 0 ; i < 5 ; i++ )
      table1.push_back( strArray[i] );
    return table1;
  } // GetTable1()

  vector<string> GetTable2() {
    string strArray[] = { ";", "(", ")", "{", "}", "+", "-", "*", "/", "%", "<", ">", ">=", "<=", "==",
                          "!=", "&", "^", "|", "=", "||", "&&", "+=", "-=", "*=", "/=", "%=", "++", "--",
                          "<<", ">>", ",", "?", ":", "[", "]", "//" };
    vector<string> table2;
    for ( int i = 0 ; i < 37 ; i++ )
      table2.push_back( strArray[i] );
    return table2;
  } // GetTable2()

  vector<string> GetTable3() {
    string strArray[] = { "void", "if", "while", "else", "do", "cin", "cout" };
    vector<string> table3;
    for ( int i = 0 ; i < 7 ; i++ )
      table3.push_back( strArray[i] );
    return table3;
  } // GetTable3()
}; // class Table

// /////////////////////////////////////////////////////////////////////////////
//                                 Global                                     //
// /////////////////////////////////////////////////////////////////////////////

typedef vector<Token> OneLineToken;
typedef vector<Ident> OneLineIdent;
Token gConstent;
Ident gIdent;
OneLineIdent gIdents;
vector<OneLineIdent> gAllIdents;
vector<OneLineToken> gAllLineToken;
vector<FunctionToken> gAllFunctionToken;
Table gTable;
Index gIndex; // index of gAllLineToken
Index gIndexOfFunctionStart;
string gType;
string gError;
bool gIsFirstLine = false;
vector<bool> gInCompound;
int gErrorLine = 0;

// /////////////////////////////////////////////////////////////////////////////
//                                 IsOO                                       //
// /////////////////////////////////////////////////////////////////////////////

bool IsTable1( string str ) {
  for ( int i = 0 ; i < gTable.mTable1.size() ; i++ )
    if ( str == gTable.mTable1[i] )
      return true;
  return false;
} // IsTable1()

bool IsTable2( string str ) {
  for ( int i = 0 ; i < gTable.mTable2.size() ; i++ )
    if ( str == gTable.mTable2[i] )
      return true;
  return false;
} // IsTable2()

bool IsTable3( string str ) {
  for ( int i = 0 ; i < gTable.mTable3.size() ; i++ )
    if ( str == gTable.mTable3[i] )
      return true;
  return false;
} // IsTable3()

bool IsANumChar( string charStr ) {
  if ( charStr.size() == 1 && charStr >= "0" && charStr <= "9" )
    return true;

  return false;
} // IsANumChar()

bool IsEnChar( string charStr ) {
  if ( charStr.size() == 1 &&
       ( ( charStr >= "a" && charStr <= "z" ) || ( charStr >= "A" && charStr <= "Z" ) ) )
    return true;
  return false;
} // IsEnChar()

bool IsString( string str ) {
  if ( str.length() > 1 && str[0] == '"' && str[str.length()-1] == '"' )
    return true;
  return false;
} // IsString()

bool RecognizedIDTokenHead( string charStr ) {
  if ( charStr == "_" || IsEnChar( charStr ) )
    return true;
  return false;
} // RecognizedIDTokenHead()

// /////////////////////////////////////////////////////////////////////////////
//                                 Print                                      //
// /////////////////////////////////////////////////////////////////////////////

void PrintOneLineToken( vector<Token> oneLineToken ) {
  for ( int i = 0; i < oneLineToken.size() ; i++ )
    cout << oneLineToken[i].mToken << " ";
} // PrintOneLineToken()

void PrintNowFunction( string str ) {
  cout << "now in " << str << endl;
} // PrintNowFunction()

void PrintUnexpectedUndeclaredToken( string str ) {
  cout << "line " << gErrorLine << " : syntax error when token is '" + str + "'" << endl;
} // PrintUnexpectedUndeclaredToken()

void PrintUnrecognizedToken( string str ) {
  cout << "line " << gErrorLine << " : syntax error when input char is '" + str + "'" << endl;
} // PrintUnrecognizedToken()

void PrintErrorMessage() {

} // PrintErrorMessage()

// /////////////////////////////////////////////////////////////////////////////
//                                GetToken                                    //
// /////////////////////////////////////////////////////////////////////////////

void GetOneLineString( string &oneLineString ) {
  // PrintNowFunction( "GetOneLineString" );
  char *str1 = new char[ 100 ];
  // cout << "> ";
  cin.getline( str1, 100 );
  oneLineString = str1;
  oneLineString += "\n";
} // GetOneLineString()

bool GetNumToken( string oneLineString, OneLineToken &oneLineToken, int &index ) {
  // PrintNowFunction( "GetNumToken" );
  string aCharToString, aTokenString;
  int dotNum = 1;
  for ( ; index < oneLineString.size() ; index++ ) {
    aCharToString = string();
    aCharToString += oneLineString[index];
    if ( aCharToString == "." )
      dotNum--;
    if ( IsANumChar( aCharToString ) || ( aCharToString == "." && dotNum > -1 ) ) {
      aTokenString += aCharToString;
    } // if
    else {
      // 所有不是數字都回傳
      index--;
      Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
      oneLineToken.push_back( token );
      return true;
    } // else
  } // for

  cout << "GetNumToken Error" << endl;
  return false;
} // GetNumToken()

bool GetIdentToken( string oneLineString, OneLineToken &oneLineToken, int &index ) {
  // PrintNowFunction( "GetIdentToken" );
  string aCharToString, aTokenString;
  for ( ; index < oneLineString.size() ; index++ ) {
    aCharToString = string();
    aCharToString += oneLineString[index];
    if ( IsANumChar( aCharToString ) || RecognizedIDTokenHead( aCharToString ) ) {
      aTokenString += aCharToString;
    } // if
    else {
      // 不是數字或英文貨底線
      index--;
      Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
      oneLineToken.push_back( token );
      return true;
    } // else
  } // for

  cout << "GetIdentToken Error" << endl;
  return false;
} // GetIdentToken()

bool GetTokenString( string &oneLineString, OneLineToken &oneLineToken, int &index, string &aTokenString ) {
  // PrintNowFunction( "GetTokenString" );
  string aCharToString;
  while ( index < oneLineString.size() && aCharToString != "\"" ) {
    aCharToString = string();
    aCharToString += oneLineString[index];
    aTokenString += aCharToString;
    index++;
  } // while

  if ( index >= oneLineString.size() ) {
    GetOneLineString( oneLineString );
    index = 0;
    return GetTokenString( oneLineString, oneLineToken, index, aTokenString );
  } // if
  else if ( aCharToString == "\"" ) { // get string
    index--;
    Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
    oneLineToken.push_back( token );
    return true;
  } // else if

  return false;
} // GetTokenString()

bool GetTokenChar( string &oneLineString, OneLineToken &oneLineToken, int &index, string &aTokenString ) {
  // PrintNowFunction( "GetTokenChar" );
  string aCharToString = "";
  aCharToString += oneLineString[index];
  if ( index < oneLineString.size() ) {
    aTokenString += aCharToString;
    index++;
    aCharToString = "";
    aCharToString += oneLineString[index];
    if ( index < oneLineString.size() && aCharToString == "'" ) {
      aTokenString += aCharToString;
      Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
      oneLineToken.push_back( token );
      return true;
    } // if
  } // if

  PrintUnrecognizedToken( aCharToString );
  return false;
} // GetTokenChar()

bool SymbolOrRecognizedToken( string oneLineString, OneLineToken &oneLineToken, int &index ) {
  // PrintNowFunction( "SymbolOrRecognizedToken" );
  string aCharToString, aTokenString;
  aCharToString = string();
  aCharToString += oneLineString[index];
  if ( IsTable2( aCharToString ) ) {
    aTokenString += aCharToString;
    index++;
    if ( index < oneLineString.size() ) {
      aCharToString += oneLineString[index];
      if ( IsTable2( aCharToString ) ) {
        aTokenString = aCharToString;
      } // if
      else
        index--;
    } // if

    Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
    oneLineToken.push_back( token );

    if ( aTokenString == "//" ) {
      aTokenString = "";
      for ( ; index < oneLineString.size() ; index++ )
        aTokenString += oneLineString[index];
      Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
      oneLineToken.push_back( token );
    } // if

    return true;
  } // if
  else if ( aCharToString == "!" ) {
    aTokenString += aCharToString;
    index++;
    if ( index < oneLineString.size() ) {
      aCharToString += oneLineString[index];
      if ( IsTable2( aCharToString ) ) {
        aTokenString = aCharToString;
        Token token( aTokenString, gAllLineToken.size(), oneLineToken.size() );
        oneLineToken.push_back( token );
        return true;
      } // if
    } // if
  } // else if

  PrintUnrecognizedToken( aCharToString );
  return false;
} // SymbolOrRecognizedToken()

bool GetOneLineToken() {
  // PrintNowFunction( "GetOneLineToken" );
  string oneLineString, aCharToString;
  OneLineToken oneLineToken;

  GetOneLineString( oneLineString );
  for ( int i = 0 ; i < oneLineString.size() ; i++ ) {
    aCharToString = string();
    aCharToString += oneLineString[i];
    if ( IsANumChar( aCharToString ) || aCharToString == "." ) {
      GetNumToken( oneLineString, oneLineToken, i );
    } // if
    else if ( RecognizedIDTokenHead( aCharToString ) ) {
      GetIdentToken( oneLineString, oneLineToken, i );
    } // else if
    else if ( aCharToString == "\"" ) {
      string aTokenString = aCharToString;
      i++;
      GetTokenString( oneLineString, oneLineToken, i, aTokenString );
    } // else if
    else if ( aCharToString == "'" ) {
      string aTokenString = aCharToString;
      i++;
      if ( !GetTokenChar( oneLineString, oneLineToken, i, aTokenString ) )
        return false;
    } // else if
    else if ( aCharToString != " " && aCharToString != "\n" && aCharToString != "\t" ) {
      if ( !SymbolOrRecognizedToken( oneLineString, oneLineToken, i ) )
        return false;
    } // else if
  } // for

  // PrintOneLineToken( oneLineToken );
  gAllLineToken.push_back( oneLineToken );
  if ( !gIsFirstLine )
    gErrorLine++;
  else
    gIsFirstLine = false;
  return true;
} // GetOneLineToken()

Token PeekCurrentToken() {
  // PrintNowFunction( "PeekCurrentToken" );
  if ( gIndex.mX > -1 )
    if ( gIndex.mX < gAllLineToken.size() && gIndex.mY + 1 < gAllLineToken[gIndex.mX].size() ) {
      // peek next token
      return gAllLineToken[gIndex.mX][gIndex.mY+1];
    } // if

  // call GetOneLineToken()
  if ( GetOneLineToken() ) {
    gIndex.mX++;
    gIndex.mY = -1;
  } // if

  return PeekCurrentToken();
} // PeekCurrentToken()

Token GetCurrentToken() {
  // PrintNowFunction( "GetCurrentToken" );
  if ( gIndex.mX > -1 )
    if ( gIndex.mX < gAllLineToken.size() && gIndex.mY + 1 < gAllLineToken[gIndex.mX].size() ) {
      // peek next token
      gIndex.mY++;
      return gAllLineToken[gIndex.mX][gIndex.mY];
    } // if

  // call GetOneLineToken()
  if ( GetOneLineToken() ) {
    gIndex.mX++;
    gIndex.mY = -1;
  } // if

  return GetCurrentToken();
} // GetCurrentToken()

void AbortCurrentLineToken() {
  gIndex.mY = gAllLineToken[gIndex.mX].size() - 1;
} // AbortCurrentLineToken()

Token PeekToken() {
  Token token = PeekCurrentToken();
  if ( token.mToken == "//" ) {
    AbortCurrentLineToken();
    token = PeekCurrentToken();
  } // if

  // cout << "Peek " << token.mToken << endl;
  return token;
} // PeekToken()

Token GetToken() {
  Token token = GetCurrentToken();
  if ( token.mToken == "//" ) {
    gIndex.mY = gAllLineToken[gIndex.mX].size() - 1;
    token = GetCurrentToken();
  } // if

  // cout << "Get " << token.mToken << endl;
  return token;
} // GetToken()

// /////////////////////////////////////////////////////////////////////////////
//                               sub function                                 //
// /////////////////////////////////////////////////////////////////////////////

bool Type_specifier() ;
bool Identifier() ;
bool Rest_of_declarators() ;
bool Constant() ;
bool Statement() ;
bool Expression() ;
bool Basic_expression() ;
bool Rest_of_maybe_logical_OR_exp() ;
bool Sign() ;
bool Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ;
void SortAllIdents() ;

void PushFunctionToken( string functionName )  {
  OneLineToken functionToken;
  int x = gIndexOfFunctionStart.mX, y = gIndexOfFunctionStart.mY;
  for ( int i = x ; i < gIndex.mX ; i++ ) {
    for ( int j = y ; j < gAllLineToken[i].size() ; j++ )
      functionToken.push_back( gAllLineToken[i][j] );
    y = 0;
  } // for

  for ( int j = 0 ; j < gIndex.mY ; j++ )
    functionToken.push_back( gAllLineToken[gIndex.mX][j] );
  FunctionToken function = FunctionToken( functionName, functionToken );
  gAllFunctionToken.push_back( function );
  gIndexOfFunctionStart = Index();
} // PushFunctionToken()

bool IdentHasDeclare( Index &index ) {
  // PrintNowFunction( "IdentHasDeclare" );
  for ( int i = 0 ; i < gAllIdents.size() ; i++ )
    for ( int j = 0 ; j < gAllIdents[i].size() ; j++ )
      if ( gIdent.mToken == gAllIdents[i][j].mToken ) {
        index.mX = i;
        index.mY = j;
        return true;
      } // if

  gError = gIdent.mToken;
  return false;
} // IdentHasDeclare()

void DeclareIdents() {
  for ( int i = 0 ; i < gIdents.size() ; i ++ ) {
    Index indexOfDeclaredIdent;
    gIdent = gIdents[i];
    if ( IdentHasDeclare( indexOfDeclaredIdent ) ) {
      gAllIdents[indexOfDeclaredIdent.mX][indexOfDeclaredIdent.mY] = gIdent;
      if ( gInCompound.size() == 0 )
        cout << "New definition of " << gIdent.mToken << " entered ..." << endl;
    } // if
    else {
      gAllIdents.back().push_back( gIdent );
      if ( gInCompound.size() == 0 )
        cout << "Definition of " << gIdent.mToken << " entered ..." << endl;
    } // else
  } // for

  SortAllIdents();
  gType = "";
} // DeclareIdents()

bool FindVariable( string identName, Ident &ident ) {
  for ( int i = 0 ; i < gAllIdents.size() ; i++ )
    for ( int j = 0 ; j < gAllIdents[i].size() ; j++ )
      if ( identName == gAllIdents[i][j].mToken ) {
        ident = gAllIdents[i][j];
        return true;
      } // if

  return false;
} // FindVariable()

void SortAllIdents() {
  for ( int i = 0 ; i < gAllIdents.size() ; i++ )
    for ( int j = 0 ; j < gAllIdents[i].size() ; j++ )
      for ( int k = j + 1 ; k < gAllIdents[i].size() ; k++ )
        if ( gAllIdents[i][j].mToken > gAllIdents[i][k].mToken ) {
          Ident tempIdent = gAllIdents[i][j];
          gAllIdents[i][j] = gAllIdents[i][k];
          gAllIdents[i][k] = tempIdent;
        } // if
} // SortAllIdents()

bool ListAllVariables() {
  // PrintNowFunction( "ListAllVariables" );
  if ( PeekToken().mToken != "ListAllVariables" )
    return false;
  GetToken();
  if ( PeekToken().mToken != "(" )
    return false;
  GetToken();
  if ( PeekToken().mToken != ")" )
    return false;
  GetToken();
  if ( PeekToken().mToken != ";" )
    return false;
  GetToken();
  for ( int i = 0 ; i < gAllIdents.size() ; i++ )
    for ( int j = 0 ; j < gAllIdents[i].size() ; j++ )
      cout << gAllIdents[i][j].mToken << endl;
  return true;
} // ListAllVariables()

bool ListVariable() {
  // PrintNowFunction( "ListVariable" );
  if ( PeekToken().mToken != "ListVariable" )
    return false;
  GetToken();
  if ( PeekToken().mToken != "(" )
    return false;
  GetToken();
  if ( !IsString( PeekToken().mToken ) )
    return false;
  string str = GetToken().mToken;
  if ( PeekToken().mToken != ")" )
    return false;
  GetToken();
  if ( PeekToken().mToken != ";" )
    return false;
  GetToken();

  if ( str.length() == 2 )
    str = "";
  else
    str = str.substr( 1, str.length() - 2 );
  Ident ident;
  FindVariable( str, ident );
  cout << ident.mType << " " << ident.mToken;
  if ( ident.mArraySize > 0 )
    cout << "[ " << ident.mArraySize << " ]";
  cout << " ;" << endl;
  return true;
} // ListVariable()

bool CIN() {
  // PrintNowFunction( "CIN" );
  if ( PeekToken().mToken != "cin" )
    return false;
  GetToken();
  return true;
} // CIN()

bool COUT() {
  // PrintNowFunction( "COUT" );
  if ( PeekToken().mToken != "cout" )
    return false;
  GetToken();
  return true;
} // COUT()

bool VOID() {
  // PrintNowFunction( "VOID" );
  if ( PeekToken().mToken != "void" )
    return false;
  GetToken();
  return true;
} // VOID()

bool RETURN() {
  // PrintNowFunction( "RETURN" );
  if ( PeekToken().mToken != "return" )
    return false;
  GetToken();
  return true;
} // RETURN()

bool IF() {
  // PrintNowFunction( "IF" );
  if ( PeekToken().mToken != "if" )
    return false;
  GetToken();
  return true;
} // IF()

bool ELSE() {
  // PrintNowFunction( "ELSE" );
  if ( PeekToken().mToken != "else" )
    return false;
  GetToken();
  return true;
} // ELSE()

bool WHILE() {
  // PrintNowFunction( "WHILE" );
  if ( PeekToken().mToken != "while" )
    return false;
  GetToken();
  return true;
} // WHILE()

bool DO() {
  // PrintNowFunction( "DO" );
  if ( PeekToken().mToken != "do" )
    return false;
  GetToken();
  return true;
} // DO()

bool PP() {
  // PrintNowFunction( "PP" );
  if ( PeekToken().mToken != "++" )
    return false;
  GetToken();
  return true;
} // PP()

bool MM() {
  // PrintNowFunction( "MM" );
  if ( PeekToken().mToken != "--" )
    return false;
  GetToken();
  return true;
} // MM()

bool TE() {
  // PrintNowFunction( "TE" );
  if ( PeekToken().mToken != "*=" )
    return false;
  GetToken();
  return true;
} // TE()

bool DE() {
  // PrintNowFunction( "DE" );
  if ( PeekToken().mToken != "/=" )
    return false;
  GetToken();
  return true;
} // DE()

bool RE() {
  // PrintNowFunction( "RE" );
  if ( PeekToken().mToken != "%=" )
    return false;
  GetToken();
  return true;
} // RE()

bool PE() {
  // PrintNowFunction( "PE" );
  if ( PeekToken().mToken != "+=" )
    return false;
  GetToken();
  return true;
} // PE()

bool ME() {
  // PrintNowFunction( "ME" );
  if ( PeekToken().mToken != "-=" )
    return false;
  GetToken();
  return true;
} // ME()

bool AND() {
  // PrintNowFunction( "AND" );
  if ( PeekToken().mToken != "&&" )
    return false;
  GetToken();
  return true;
} // AND()

bool OR() {
  // PrintNowFunction( "OR" );
  if ( PeekToken().mToken != "||" )
    return false;
  GetToken();
  return true;
} // OR()

bool EQ() {
  // PrintNowFunction( "EQ" );
  if ( PeekToken().mToken != "==" )
    return false;
  GetToken();
  return true;
} // EQ()

bool NEQ() {
  // PrintNowFunction( "NEQ" );
  if ( PeekToken().mToken != "!=" )
    return false;
  GetToken();
  return true;
} // NEQ()

bool LE() {
  // PrintNowFunction( "LE" );
  if ( PeekToken().mToken != "<=" )
    return false;
  GetToken();
  return true;
} // LE()

bool GE() {
  // PrintNowFunction( "GE" );
  if ( PeekToken().mToken != ">=" )
    return false;
  GetToken();
  return true;
} // GE()

bool LS() {
  // PrintNowFunction( "LS" );
  if ( PeekToken().mToken != "<<" )
    return false;
  GetToken();
  return true;
} // LS()

bool RS() {
  // PrintNowFunction( "RS" );
  if ( PeekToken().mToken != ">>" )
    return false;
  GetToken();
  return true;
} // RS()

// /////////////////////////////////////////////////////////////////////////////
//                              compound_statement                            //
// /////////////////////////////////////////////////////////////////////////////

bool Declaration() {
  // PrintNowFunction( "Declaration" );
  if ( !Type_specifier() )
    return false;
  if ( !Identifier() )
    return false;
  GetToken();
  gIdents.push_back( gIdent );
  if ( !Rest_of_declarators() )
    return false;

  return true;
} // Declaration()

// /////////////////////////////////////////////////////////////////////////////
//                      function_definition_or_declarators                    //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_declarators() {
  // PrintNowFunction( "Rest_of_declarators" );
  if ( PeekToken().mToken == "[" ) {
    GetToken();
    if ( !Constant() )
      return false;
    gIdents[gIdents.size()-1].mArraySize = atoi( gConstent.mToken.c_str() );
    if ( PeekToken().mToken != "]" )
      return false;
    GetToken();
  } // if

  while ( PeekToken().mToken == "," ) {
    GetToken();
    if ( !Identifier() )
      return false;
    GetToken();
    gIdents.push_back( gIdent );
    if ( PeekToken().mToken == "[" ) {
      GetToken();
      if ( !Constant() )
        return false;
      gIdents[gIdents.size()-1].mArraySize = atoi( gConstent.mToken.c_str() );
      if ( PeekToken().mToken != "]" )
        return false;
      GetToken();
    } // if
  } // while

  if ( PeekToken().mToken != ";" )
    return false;
  GetToken();
  DeclareIdents();
  return true;
} // Rest_of_declarators()

// /////////////////////////////////////////////////////////////////////////////
//                            formal_parameter_list                           //
// /////////////////////////////////////////////////////////////////////////////

bool Constant() {
  // PrintNowFunction( "Constant" );
  string mToken = PeekToken().mToken, aCharToString = "";
  aCharToString += mToken[0];
  if ( IsEnChar( aCharToString ) || IsTable1( mToken ) || IsTable2( mToken ) || IsTable3( mToken ) )
    return false;
  gConstent = GetToken();
  return true;
} // Constant()

// /////////////////////////////////////////////////////////////////////////////
//                        function_definition_without_ID                      //
// /////////////////////////////////////////////////////////////////////////////

bool Formal_parameter_list() {
  // PrintNowFunction( "Formal_parameter_list" );
  if ( !Type_specifier() )
    return false;
  if ( PeekToken().mToken == "&" )
    GetToken();
  if ( !Identifier() )
    return false;
  GetToken();
  gIdents.push_back( gIdent );
  if ( PeekToken().mToken == "[" ) {
    GetToken();
    if ( !Constant() )
      return false;
    gIdents[gIdents.size()-1].mArraySize = atoi( gConstent.mToken.c_str() );
    if ( PeekToken().mToken != "]" )
      return false;
    GetToken();
  } // if

  if ( PeekToken().mToken == "," ) {
    GetToken();
    return Formal_parameter_list();
  } // if

  return true;
} // Formal_parameter_list()

bool Compound_statement() {
  // PrintNowFunction( "Compound_statement" );
  if ( PeekToken().mToken != "{" )
    return false;
  GetToken();
  OneLineIdent idents;
  gAllIdents.push_back( idents );
  gInCompound.push_back( true );
  while ( Declaration() || Statement() ) {
    // do nothing
  } // while

  if ( PeekToken().mToken != "}" )
    return false;
  GetToken();
  gAllIdents.pop_back();
  gInCompound.pop_back();
  return true;
} // Compound_statement()

// /////////////////////////////////////////////////////////////////////////////
//                                definition                                  //
// /////////////////////////////////////////////////////////////////////////////

bool Identifier() {
  // PrintNowFunction( "Identifier" );
  string mToken = PeekToken().mToken, aCharToString = "";
  aCharToString += mToken[0];
  if ( !IsEnChar( aCharToString ) || IsTable1( mToken ) || IsTable2( mToken ) || IsTable3( mToken ) )
    return false;
  gIdent = Ident( PeekToken(), gType );
  return true;
} // Identifier()

bool Function_definition_without_ID() {
  // PrintNowFunction( "Function_definition_without_ID" );
  string functionName = gIdent.mToken;
  if ( PeekToken().mToken != "(" )
    return false;
  GetToken();
  if ( VOID() || Formal_parameter_list() ) {
    if ( gIdents.size() > 0 )
      DeclareIdents();
  } // if

  if ( PeekToken().mToken != ")" )
    return false;
  GetToken();
  if ( !Compound_statement() )
    return false;
  PushFunctionToken( functionName );
  return true;
} // Function_definition_without_ID()

bool Type_specifier() {
  // PrintNowFunction( "Type_specifier" );
  if ( !IsTable1( PeekToken().mToken ) )
    return false;
  gType = GetToken().mToken;
  return true;
} // Type_specifier()

bool Function_definition_or_declarators() {
  // PrintNowFunction( "Function_definition_or_declarators" );
  if ( !Function_definition_without_ID() && !Rest_of_declarators() )
    return false;

  return true;
} // Function_definition_or_declarators()


// /////////////////////////////////////////////////////////////////////////////
//                           non_comma_expression                             //
// /////////////////////////////////////////////////////////////////////////////

// bool Rest_of_non_comma_expression() {
//   // PrintNowFunction( "Rest_of_non_comma_expression" );
//   if ( PeekToken().mToken != "?" )
//     return false;
//   GetToken();
//   if ( !Expression() )
//     return false;
//   if ( PeekToken().mToken != ":" )
//     return false;
//   GetToken();
//   if ( !Basic_expression() )
//     return false;
//   if ( Rest_of_non_comma_expression() ) {
//     // do nothing
//   } // if
//
//   return true;
// } // Rest_of_non_comma_expression()

// /////////////////////////////////////////////////////////////////////////////
//                           actual_parameter_list                            //
// /////////////////////////////////////////////////////////////////////////////

// bool Non_comma_expression() {
//   // PrintNowFunction( "Non_comma_expression" );
//   if ( !Basic_expression() )
//     return false;
//   if ( Rest_of_non_comma_expression() ) {
//     // do nothing
//   } // if
//
//   return true;
// } // Non_comma_expression()

// /////////////////////////////////////////////////////////////////////////////
//                    rest_of_Identifier_started_basic_exp                    //
// /////////////////////////////////////////////////////////////////////////////

bool Actual_parameter_list() {
  // PrintNowFunction( "Actual_parameter_list" );
  if ( !Basic_expression() )
    return false;
  while ( PeekToken().mToken == "," ) {
    GetToken();
    if ( !Basic_expression() )
      return false;
  } // while

  return true;
} // Actual_parameter_list()

bool Assignment_operator() {
  // PrintNowFunction( "Assignment_operator" );
  if ( PeekToken().mToken == "=" )
    GetToken();
  else if ( !TE() && !DE() && !RE() && !PE() && !ME() )
    return false;
  return true;
} // Assignment_operator()

// /////////////////////////////////////////////////////////////////////////////
//                          signed_basic_expression                           //
// /////////////////////////////////////////////////////////////////////////////

// bool Rest_of_Identifier_started_signed_basic_exp() {
//   // PrintNowFunction( "Rest_of_Identifier_started_signed_basic_exp" );
//   if ( PeekToken().mToken == "[" ) {
//     if ( !Expression() )
//       return false;
//     if ( PeekToken().mToken != "]" )
//       return false;
//     GetToken();
//     if ( PP() || MM() ) {
//       // do nothing
//     } // if
//
//     if ( !Rest_of_maybe_logical_OR_exp() )
//       return false;
//   } // if
//   else if ( PP() || MM() ) {
//     if ( !Rest_of_maybe_logical_OR_exp() )
//       return false;
//   } // else if
//   else if ( Rest_of_maybe_logical_OR_exp() ) {
//     // do nothing
//   } // else if
//   else if ( PeekToken().mToken == "(" ) {
//     GetToken();
//     if ( Actual_parameter_list() ) {
//       // do nothing
//     } // if
//
//     if ( PeekToken().mToken != ")" )
//       return false;
//     GetToken();
//     if ( !Rest_of_maybe_logical_OR_exp() )
//       return false;
//   } // else if
//   else
//     return false;
//
//   return true;
// } // Rest_of_Identifier_started_signed_basic_exp()


// /////////////////////////////////////////////////////////////////////////////
//                               signed_unary_exp                             //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_Identifier_started_unary_exp() {
  // PrintNowFunction( "Rest_of_Identifier_started_unary_exp" );
  if ( PeekToken().mToken == "(" ) {
    GetToken();
    if ( Actual_parameter_list() ) {
      // do nothing
    } // if

    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
  } // if
  else if ( PeekToken().mToken == "[" ) {
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != "]" )
      return false;
    GetToken();
    if ( PP() || MM() ) {
      // do nothing()
    } // if
  } // else if
  else if ( PP() || MM() ) {
    // do nothing
  } // else if

  return true;
} // Rest_of_Identifier_started_unary_exp()

// /////////////////////////////////////////////////////////////////////////////
//                                  unary_exp                                 //
// /////////////////////////////////////////////////////////////////////////////

bool Signed_unary_exp() {
  // PrintNowFunction( "Signed_unary_exp" );
  if ( Identifier() ) {
    Index indexOfDeclaredIdent;
    if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
      return false;
    GetToken();
    if ( Rest_of_Identifier_started_unary_exp() ) {
      // do nothing
    } // if
  } // if
  else if ( Constant() ) {
    // do nothing
  } // else if
  else if ( PeekToken().mToken == "(" ) {
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
  } // else if
  else
    return false;
  return true;
} // Signed_unary_exp()

bool Unsigned_unary_exp() {
  if ( Identifier() ) {
    if ( PeekToken().mToken == "(" ) { // 1
      GetToken();
      if ( !Actual_parameter_list() )
        return false;
      if ( PeekToken().mToken != ")" )
        return false;
      GetToken();
    } // if
    else if ( PeekToken().mToken == "[" ) { // 2
      GetToken();
      if ( !Expression() )
        return false;
      if ( PeekToken().mToken != ")" )
        return false;
      GetToken();
      if ( PP() || MM() ) { // 3
        // do nothing
      } // else if
    } // else if
    else if ( PP() || MM() ) { // 3
      // do nothing
    } // else if
  } // if
  else if ( Constant() ) {
    // do nothing
  } // else if
  else if ( PeekToken().mToken == "(" ) {
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
  } // else if
  else
    return false;

  return true;
} // Unsigned_unary_exp()

// /////////////////////////////////////////////////////////////////////////////
//                            rest_of_maybe_mult_exp                          //
// /////////////////////////////////////////////////////////////////////////////

bool Unary_exp() {
  // PrintNowFunction( "Unary_exp" );
  if ( Sign() ) {
    while ( Sign() ) {
      // do nothing
    } // while

    if ( !Signed_unary_exp() )
      return false;
  } // if
  else if ( Unsigned_unary_exp() ) {
    // do nothing
  } // else if
  else if ( PP() || MM() ) {
    if ( !Identifier() )
      return false;
    Index indexOfDeclaredIdent;
    if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
      return false;
    GetToken();
    if ( PeekToken().mToken == "[" ) {
      GetToken();
      if ( !Expression() )
        return false;
      if ( PeekToken().mToken != "]" )
        return false;
      GetToken();
    } // if
  } // else if
  else
    return false;
  return true;
} // Unary_exp()

// /////////////////////////////////////////////////////////////////////////////
//                          rest_of_maybe_additive_exp                        //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_mult_exp() {
  // PrintNowFunction( "Rest_of_maybe_mult_exp" );
  while ( PeekToken().mToken == "*" || PeekToken().mToken == "/" || PeekToken().mToken == "%" ) {
    GetToken();
    if ( !Unary_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_mult_exp()

bool Maybe_mult_exp() {
  // PrintNowFunction( "Maybe_mult_exp" );
  if ( !Unary_exp() )
    return false;
  if ( !Rest_of_maybe_mult_exp() )
    return false;
  return true;
} // Maybe_mult_exp()

// /////////////////////////////////////////////////////////////////////////////
//                           rest_of_maybe_shift_exp                          //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_additive_exp() {
  // PrintNowFunction( "Rest_of_maybe_additive_exp" );
  if ( !Rest_of_maybe_mult_exp() )
    return false;
  while ( PeekToken().mToken == "+" || PeekToken().mToken == "-" ) {
    GetToken();
    if ( !Maybe_mult_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_additive_exp()

bool Maybe_additive_exp() {
  // PrintNowFunction( "Maybe_additive_exp" );
  if ( !Maybe_mult_exp() )
    return false;
  while ( PeekToken().mToken == "+" || PeekToken().mToken == "-" ) {
    GetToken();
    if ( !Maybe_mult_exp() )
      return false;
  } // while

  return true;
} // Maybe_additive_exp()

// /////////////////////////////////////////////////////////////////////////////
//                         rest_of_maybe_relational_exp                       //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_shift_exp() {
  // PrintNowFunction( "Rest_of_maybe_shift_exp" );
  if ( !Rest_of_maybe_additive_exp() )
    return false;
  while ( LS() || RS() )
    if ( !Maybe_additive_exp() )
      return false;
  return true;
} // Rest_of_maybe_shift_exp()

bool Maybe_shift_exp() {
  // PrintNowFunction( "Maybe_shift_exp" );
  if ( !Maybe_additive_exp() )
    return false;
  while ( LS() || RS() )
    if ( !Maybe_additive_exp() )
      return false;
  return true;
} // Maybe_shift_exp()

// /////////////////////////////////////////////////////////////////////////////
//                          rest_of_maybe_equality_exp                        //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_relational_exp() {
  // PrintNowFunction( "Rest_of_maybe_relational_exp" );
  if ( !Rest_of_maybe_shift_exp() )
    return false;
  while ( PeekToken().mToken == "<" || PeekToken().mToken == ">" || LE() || GE() ) {
    if ( PeekToken().mToken == "<" || PeekToken().mToken == ">" )
      GetToken();
    if ( !Maybe_shift_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_relational_exp()

bool Maybe_relational_exp() {
  // PrintNowFunction( "Maybe_relational_exp" );
  if ( !Maybe_shift_exp() )
    return false;
  while ( PeekToken().mToken == "<" || PeekToken().mToken == ">" || LE() || GE() ) {
    if ( PeekToken().mToken == "<" || PeekToken().mToken == ">" )
      GetToken();
    if ( !Maybe_shift_exp() )
      return false;
  } // while

  return true;
} // Maybe_relational_exp()

// /////////////////////////////////////////////////////////////////////////////
//                          rest_of_maybe_bit_AND_exp                         //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_equality_exp() {
  // PrintNowFunction( "Rest_of_maybe_equality_exp" );
  if ( !Rest_of_maybe_relational_exp() )
    return false;
  while ( EQ() || NEQ() )
    if ( !Maybe_relational_exp() )
      return false;
  return true;
} // Rest_of_maybe_equality_exp()

bool Maybe_equality_exp() {
  // PrintNowFunction( "Maybe_equality_exp" );
  if ( !Maybe_relational_exp() )
    return false;
  while ( EQ() || NEQ() )
    if ( !Maybe_relational_exp() )
      return false;
  return true;
} // Maybe_equality_exp()

// /////////////////////////////////////////////////////////////////////////////
//                         rest_of_maybe_bit_ex_OR_exp                        //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_bit_AND_exp() {
  // PrintNowFunction( "Rest_of_maybe_bit_AND_exp" );
  if ( !Rest_of_maybe_equality_exp() )
    return false;
  while ( PeekToken().mToken == "&" ) {
    GetToken();
    if ( !Maybe_equality_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_bit_AND_exp()

bool Maybe_bit_AND_exp() {
  // PrintNowFunction( "Maybe_bit_AND_exp" );
  if ( !Maybe_equality_exp() )
    return false;
  while ( PeekToken().mToken == "&" ) {
    GetToken();
    if ( !Maybe_equality_exp() )
      return false;
  } // while

  return true;
} // Maybe_bit_AND_exp()

// /////////////////////////////////////////////////////////////////////////////
//                          rest_of_maybe_bit_OR_exp                          //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_bit_ex_OR_exp() {
  // PrintNowFunction( "Rest_of_maybe_bit_ex_OR_exp" );
  if ( !Rest_of_maybe_bit_AND_exp() )
    return false;
  while ( PeekToken().mToken == "^" ) {
    GetToken();
    if ( !Maybe_bit_AND_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_bit_ex_OR_exp()

bool Maybe_bit_ex_OR_exp() {
  // PrintNowFunction( "Maybe_bit_ex_OR_exp" );
  if ( !Maybe_bit_AND_exp() )
    return false;
  while ( PeekToken().mToken == "^" ) {
    GetToken();
    if ( !Maybe_bit_AND_exp() )
      return false;
  } // while

  return true;
} // Maybe_bit_ex_OR_exp()

// /////////////////////////////////////////////////////////////////////////////
//                       rest_of_maybe_logical_AND_exp                        //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_bit_OR_exp() {
  // PrintNowFunction( "Rest_of_maybe_bit_OR_exp" );
  if ( !Rest_of_maybe_bit_ex_OR_exp() )
    return false;
  while ( PeekToken().mToken == "|" ) {
    GetToken();
    if ( !Maybe_bit_ex_OR_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_bit_OR_exp()

bool Maybe_bit_OR_exp() {
  // PrintNowFunction( "Maybe_bit_OR_exp" );
  if ( !Maybe_bit_ex_OR_exp() )
    return false;
  while ( PeekToken().mToken == "|" ) {
    GetToken();
    if ( !Maybe_bit_ex_OR_exp() )
      return false;
  } // while

  return true;
} // Maybe_bit_OR_exp()

// /////////////////////////////////////////////////////////////////////////////
//                        rest_of_maybe_logical_OR_exp                        //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_maybe_logical_AND_exp() {
  // PrintNowFunction( "Rest_of_maybe_logical_AND_exp" );
  if ( !Rest_of_maybe_bit_OR_exp() )
    return false;
  while ( AND() )
    if ( !Maybe_bit_OR_exp() )
      return false;
  return true;
} // Rest_of_maybe_logical_AND_exp()

bool Maybe_logical_AND_exp() {
  // PrintNowFunction( "Maybe_logical_AND_exp" );
  if ( !Maybe_bit_OR_exp() )
    return false;
  while ( AND() )
    if ( !Maybe_bit_OR_exp() )
      return false;
  return true;
} // Maybe_logical_AND_exp()

// /////////////////////////////////////////////////////////////////////////////
//                             basic_expression                               //
// /////////////////////////////////////////////////////////////////////////////

bool Rest_of_Identifier_started_basic_exp() {
  // PrintNowFunction( "Rest_of_Identifier_started_basic_exp" );
  if ( PeekToken().mToken == "[" ) { // 1
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != "]" )
      return false;
    GetToken();
    if ( Assignment_operator() ) {
      if ( !Basic_expression() )
        return false;
    } // if
    else if ( PP() || MM() ) {
      if ( !Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() )
        return false;
    } // else if
    else if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) {
      // do nothing
    } // else if
    else
      return false;
  } // if
  else if ( Assignment_operator() ) { // 2
    if ( !Basic_expression() )
      return false;
  } // else if
  else if ( PP() || MM() ) { // 3
    if ( !Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() )
      return false;
  } // else if
  else if ( Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() ) { // 4
    // do nothing
  } // else if
  else if ( PeekToken().mToken == "(" ) { // 5
    GetToken();
    if ( Actual_parameter_list() ) {
      // do nothing
    } // if

    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
    if ( !Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() )
      return false;
  } // else if
  else
    return false;

  return true;
} // Rest_of_Identifier_started_basic_exp()

bool Rest_of_PPMM_Identifier_started_basic_exp() {
  // PrintNowFunction( "Rest_of_PPMM_Identifier_started_basic_exp" );
  if ( PeekToken().mToken == "[" ) {
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != "]" )
      return false;
    GetToken();
  } // if

  if ( !Rest_of_maybe_logical_OR_exp() )
    return false;

  return true;
} // Rest_of_PPMM_Identifier_started_basic_exp()

bool Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() {
  // Romce_and_romloe()
  if ( !Rest_of_maybe_logical_OR_exp() )
    return false;
  if ( PeekToken().mToken == "?" ) {
    GetToken();
    if ( !Basic_expression() )
      return false;
    if ( PeekToken().mToken != ":" )
      return false;
    GetToken();
    if ( !Basic_expression() )
      return false;
  } // if

  return true;
} // Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp()

// bool Signed_basic_expression() {
//   // PrintNowFunction( "Signed_basic_expression" );
//   if ( Identifier() ) {
//     Index indexOfDeclaredIdent;
//     if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
//       return false;
//     GetToken();
//     if ( !Rest_of_Identifier_started_signed_basic_exp() )
//       return false;
//   } // if
//   else if ( Constant() ) {
//     if ( !Rest_of_maybe_logical_OR_exp() )
//       return false;
//   } // else if
//   else if ( PeekToken().mToken == "(" ) {
//     GetToken();
//     if ( !Expression() )
//       return false;
//     if ( PeekToken().mToken != ")" )
//       return false;
//     GetToken();
//     if ( !Rest_of_maybe_logical_OR_exp() )
//       return false;
//   } // else if
//   else
//     return false;
//
//   return true;
// } // Signed_basic_expression()

bool Sign() {
  // PrintNowFunction( "Sign" );
  if ( PeekToken().mToken != "+" && PeekToken().mToken != "-" && PeekToken().mToken != "!" )
    return false;
  GetToken();
  return true;
} // Sign()

bool Rest_of_maybe_logical_OR_exp() {
  // PrintNowFunction( "Rest_of_maybe_logical_OR_exp" );
  if ( !Rest_of_maybe_logical_AND_exp() )
    return false;
  while ( OR() ) {
    if ( !Maybe_logical_AND_exp() )
      return false;
  } // while

  return true;
} // Rest_of_maybe_logical_OR_exp()

// /////////////////////////////////////////////////////////////////////////////
//                                expression                                  //
// /////////////////////////////////////////////////////////////////////////////

bool Basic_expression() {
  // PrintNowFunction( "Basic_expression" );
  if ( Identifier() ) {
    Index indexOfDeclaredIdent;
    if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
      return false;
    GetToken();
    if ( !Rest_of_Identifier_started_basic_exp() )
      return false;
  } // if
  else if ( PP() || MM() ) {
    if ( !Identifier() )
      return false;
    Index indexOfDeclaredIdent;
    if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
      return false;
    GetToken();
    if ( !Rest_of_PPMM_Identifier_started_basic_exp() )
      return false;
  } // else if
  else if ( Sign() ) {
    while ( Sign() ) {
      // do nothing
    } // while

    if ( !Signed_unary_exp() )
      return false;
    if ( !Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() )
      return false;
  } // else if
  else if ( Constant() ) {
    if ( !Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() )
      return false;
  } // else if
  else if ( PeekToken().mToken == "(" ) {
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
    if ( !Rest_of_maybe_conditional_exp_and_rest_of_maybe_logical_OR_exp() )
      return false;
  } // else if
  else
    return false;

  return true;
} // Basic_expression()

// bool Rest_of_expression() {
//   // PrintNowFunction( "Rest_of_expression" );
//   if ( PeekToken().mToken == "," ) {
//     GetToken();
//     if ( !Basic_expression() )
//       return false;
//     if ( Rest_of_expression() ) {
//       // do nothing
//     } // if
//   } // if
//   else if ( PeekToken().mToken == "?" ) {
//     GetToken();
//     if ( !Expression() )
//       return false;
//     if ( PeekToken().mToken != ":" )
//       return false;
//     GetToken();
//     if ( !Basic_expression() )
//       return false;
//     if ( Rest_of_expression() ) {
//       // do nothing
//     } // if
//   } // else if
//   else
//     return false;
//
//   return true;
// } // Rest_of_expression()

// /////////////////////////////////////////////////////////////////////////////
//                                statement                                   //
// /////////////////////////////////////////////////////////////////////////////

bool Expression() {
  // PrintNowFunction( "Expression" );
  if ( !Basic_expression() )
    return false;
  while ( PeekToken().mToken == "," ) {
    GetToken();
    if ( !Basic_expression() )
      return false;
  } // while

  return true;
} // Expression()

// /////////////////////////////////////////////////////////////////////////////
//                                user_input                                  //
// /////////////////////////////////////////////////////////////////////////////

bool Definition() {
  // PrintNowFunction( "Definition" );
  gIndexOfFunctionStart = gIndex;
  if ( VOID() ) {
    if ( !Identifier() )
      return false;
    GetToken();
    if ( !Function_definition_without_ID() )
      return false;
  } // if
  else if ( Type_specifier() ) {
    if ( !Identifier() )
      return false;
    GetToken();
    gIdents.push_back( gIdent );
    if ( !Function_definition_or_declarators() )
      return false;
  } // else if
  else
    return false;

  return true;
} // Definition()

bool Statement() {
  // PrintNowFunction( "Statement" );
  if ( PeekToken().mToken == ";" ) {
    GetToken();
  } // if
  else if ( RETURN() ) {
    if ( Expression() ) // 0 or 1
      if ( PeekToken().mToken != ";" )
        return false;
    if ( PeekToken().mToken != ";" )
      return false;
    GetToken();
  } // else if
  else if ( Compound_statement() ) {
    // do nothing
  } // else if
  else if ( IF() ) {
    if ( PeekToken().mToken != "(" )
      return false;
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
    gInCompound.push_back( true );
    if ( !Statement() )
      return false;
    if ( ELSE() ) // 0 or 1
      if ( !Statement() )
        return false;
    gInCompound.pop_back();
  } // if
  else if ( WHILE() ) {
    if ( PeekToken().mToken != "(" )
      return false;
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
    gInCompound.push_back( true );
    if ( !Statement() )
      return false;
    gInCompound.pop_back();
  } // else if
  else if ( DO() ) {
    gInCompound.push_back( true );
    if ( !Statement() )
      return false;
    gInCompound.pop_back();
    if ( !WHILE() )
      return false;
    if ( PeekToken().mToken != "(" )
      return false;
    GetToken();
    if ( !Expression() )
      return false;
    if ( PeekToken().mToken != ")" )
      return false;
    GetToken();
    if ( PeekToken().mToken != ";" )
      return false;
    GetToken();
  } // else if
  else if ( CIN() ) {
    if ( PeekToken().mToken != ">>" )
      return false;
    GetToken();
    if ( !Identifier() )
      return false;
    Index indexOfDeclaredIdent;
    if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
      return false;
    GetToken();
    while ( PeekToken().mToken == ">>" ) {
      GetToken();
      if ( !Identifier() )
        return false;
      Index indexOfDeclaredIdent;
      if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
        return false;
      GetToken();
    } // while

    if ( PeekToken().mToken != ";" )
      return false;
    GetToken();
  } // else if
  else if ( COUT() ) {
    if ( PeekToken().mToken != "<<" )
      return false;
    GetToken();
    if ( Identifier() ) {
      Index indexOfDeclaredIdent;
      if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
        return false;
      GetToken();
    } // if
    else if ( Constant() ) {
      // do nothing
    } // else if
    else
      return false;
    while ( PeekToken().mToken == "<<" || PeekToken().mToken == "+" ) {
      GetToken();
      if ( Identifier() ) {
        Index indexOfDeclaredIdent;
        if ( !IdentHasDeclare( indexOfDeclaredIdent ) )
          return false;
        GetToken();
      } // if
      else if ( Constant() ) {
        // do nothing
      } // else if
      else
        return false;
    } // while

    if ( PeekToken().mToken != ";" )
      return false;
    GetToken();
  } // else if
  else if ( ListAllVariables() ) {
    // do nothing
  } // else if
  else if ( ListVariable() ) {
    // do nothing
  } // else if
  else if ( Expression() ) {
    if ( PeekToken().mToken != ";" )
      return false;
    GetToken();
  } // else if
  else
    return false;

  if ( gInCompound.size() == 0 )
    cout << "Statement executed ..." << endl;
  return true;
} // Statement()

// /////////////////////////////////////////////////////////////////////////////
//                                   start                                    //
// /////////////////////////////////////////////////////////////////////////////

bool Done() {
  // PrintNowFunction( "Done" );
  if ( PeekToken().mToken != "Done" )
    return false;
  GetToken();
  if ( PeekToken().mToken != "(" )
    return false;
  GetToken();
  if ( PeekToken().mToken != ")" )
    return false;
  GetToken();
  if ( PeekToken().mToken != ";" )
    return false;

  cout << "Our-C exited ..." << endl;
  return true;
} // Done()

bool Run() {
  // PrintNowFunction( "Run" );
  OneLineIdent idents;
  gAllIdents.push_back( idents );

  cout << "> ";
  while ( !Done() ) {
    if ( Definition() || Statement() ) {
      gError = "";
    } // if
    else {
      if ( gError == "" )
        gError = PeekToken().mToken;
      PrintUnexpectedUndeclaredToken( gError );
      gError = "";
      gInCompound.resize( 0 );
      AbortCurrentLineToken();
    } // else

    gErrorLine = 1;
    gIsFirstLine = true;
    gIdents = OneLineIdent();
    cout << "> ";
  } // while

  return true;
} // Run()

int main() {
  string e, testNum;
  cout << "Our-C running ..." << endl;
  char *str1 = new char[ 100 ];
  cin.getline( str1, 100 );
  testNum = str1;
  uTestNum  = atof( testNum.c_str() );

  Run();
  // while ( PeekToken().mToken != "Done" ) {
  //   cout << GetToken().mToken << endl;
  // } // while

  return 0;
} // main()
