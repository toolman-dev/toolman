/**
 * A Toolman grammar for ANTLR v4.
 * 
 * @author taoyu @author taowei
 */
parser grammar ToolmanParser;

options {
	tokenVocab = ToolmanLexer;
}

document: (importStatement | decl)* EOF;

// import {someType as aliasName, ...} from '/path/to/xx.tm'
importStatement:
	Import OpenBrace identifierName (As identifierName)? (
		Comma identifierName (As identifierName)?
	)* CloseBrace From StringLiteral SemiColon;

decl: typeDecl | apiDecl;

typeDecl:
	Type (
		signleTypeDecl
		| (
			OpenParen signleTypeDecl (Comma signleTypeDecl)* CloseParen
		)
	);

signleTypeDecl: structDecl | enumDecl;

apiDecl: Api; // TODO

structDecl:
	Pub? identifierName Struct OpenBrace structFieldList* CloseBrace;

enumDecl:
	Pub? identifierName Enum OpenBrace enumFieldList+ CloseBrace;

/// toolman types
type_:
	(String | I32 | I64 | U32 | U64 | Float | Bool | Any)	# PrimitiveType
	// [SomeType]
	| OpenBracket listElementType CloseBracket				# ListType
	// {keyType: valueType}
	| OpenBrace mapKeyType Colon mapValueType CloseBrace	# MapType
	// (fieldName: SomeType|fieldName: OtherType)
	| OpenParen structField (Or structField)+ CloseParen	# OneofType
	| identifierName										# CustomTypeName;

listElementType: type_;

mapKeyType: type_;

mapValueType: type_;

fieldType: type_;

structFieldList: structField (Comma structField)*;

structField:
	identifierName Colon fieldType QuestionMark? (
		Assign structFieldInit
	)?;

structFieldInit: literal | enumItem;

enumFieldList: enumField (Comma enumField)*;

enumField: identifierName Assign intgerLiteral;

// SomeEnum::Item
enumItem: identifierName Doublecolon identifierName;

identifierName: Identifier | reservedWord;

reservedWord: keyword | BooleanLiteral;

keyword:
	Struct
	| Enum
	| Pub
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
	| Float;

literal: primitiveLiteral | listLiteral | mapLiteral;

primitiveLiteral:
	BooleanLiteral
	| StringLiteral
	| numericLiteral;

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

listLiteral: OpenBracket listElementList? CloseBracket;

listElementList: listElement (Comma listElement)*;

listElement: literal Comma?;

mapLiteral:
	OpenBrace primitiveLiteral Colon literal (
		Comma primitiveLiteral Colon literal
	)* CloseBrace;
