/**
 * A Toolman grammar for ANTLR v4.
 * 
 * @author taoyu
 * @author taowei
 */
parser grammar ToolmanParser;

options {
	tokenVocab = ToolmanLexer;
}

document: importStatement* (optionStatement | decl)* EOF;

// from '/pathxxx.tm' import someType as aliasName, ...
importStatement: From StringLiteral Import importList SemiColon;

importList:
	importName (As importNameAlias)? (
		Comma importName (As importNameAlias)?
	)*		# FromImport
	| Star	# FromImportStar;

importName: identifierName;

importNameAlias: identifierName;

//
// Option
// 
//
// 
//

optionStatement:
	Option identifierName Assign optionValue SemiColon;

optionValue: BooleanLiteral | numericLiteral | StringLiteral;

decl: typeDecl | apiDecl;

typeDecl:
	Type (
		signleTypeDecl
		| (
			OpenParen signleTypeDecl (Comma signleTypeDecl)* CloseParen
		)
	);

signleTypeDecl: structDecl | enumDecl;

apiDecl:
	Api identifierName (
		signleApiDecl
		| OpenParen signleApiDecl (Comma signleApiDecl)* CloseParen
	);

signleApiDecl:
	httpMethod path OpenParen identifierName CloseParen apiReturns;

apiReturns: Returns OpenBrace returnsList CloseBrace;

returnsList: returnsItem (Comma returnsItem)*;

returnsItem:
	DecIntegerLiteral Arrow (
		identifierName
		| OpenBrace structFieldList* OpenBrace
	);

httpMethod:
	Get
	| Post
	| Delete
	| Put
	| Patch
	| Head
	| Options
	| Trace
	| Connect;

path: (Slash? pathParam? pathString pathParam?)+;

pathParam: OpenBrace structField CloseBrace;

pathString: PathString | identifierName;

structDecl:
	identifierName Struct OpenBrace structFieldList* CloseBrace;

enumDecl:
	identifierName Enum OpenBrace enumFieldList+ CloseBrace;

/// toolman types
type_:
	(String | I32 | I64 | U32 | U64 | Float | Bool | Any) # PrimitiveType
	// [SomeType]
	| OpenBracket listElementType CloseBracket # ListType
	// {keyType: valueType}
	| OpenBrace mapKeyType Colon mapValueType CloseBrace # MapType
	// (fieldName: SomeType|fieldName: OtherType)
	| OpenParen structField (Or structField)+ CloseParen	# OneofType
	| identifierName										# CustomTypeName;

listElementType: type_;

mapKeyType: type_;

mapValueType: type_;

fieldType: type_;

structFieldList: structField (Comma structField)*;

structField: DocumentComment* InlineComment? identifierName Colon fieldType QuestionMark? InlineComment?;

enumFieldList: enumField (Comma enumField)*;

enumField: DocumentComment* InlineComment? identifierName Assign intgerLiteral InlineComment?;

// SomeEnum::Item
enumItem: identifierName Doublecolon identifierName;

identifierName: Identifier | reservedWord;

reservedWord: keyword | BooleanLiteral;

keyword:
	Struct
	| Enum
	| Import
	| As
	| From
	| Type
	| Api
	| Any
	| Bool
	| String
	| I32
	| I64
	| U32
	| U64
	| Float
	| Returns
	| httpMethod;

intgerLiteral:
	DecIntegerLiteral
	| HexIntegerLiteral
	| OctalIntegerLiteral
	| BinaryIntegerLiteral;

numericLiteral:
	DecIntegerLiteral
	| DecimalLiteral
	| HexIntegerLiteral
	| OctalIntegerLiteral
	| BinaryIntegerLiteral;
