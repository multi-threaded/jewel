package org.jewel.frontend;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Lexer {

    public record Token(Token.Type type, String lexeme, int line, int column) {

        public enum Type {
            KEYWORD, LITERAL, OPERATOR, IDENTIFIER, GROUPING, EOF;

            @Override
            public String toString() { return name(); }
        }

    }

    // TODO: implement correct regex patterns for tokenization
    public static final Pattern KEYWORD_PATTERN = Pattern.compile("");
    public static final Pattern LITERAL_PATTERN = Pattern.compile("");
    public static final Pattern OPERATOR_PATTERN = Pattern.compile("");
    public static final Pattern IDENTIFIER_PATTERN = Pattern.compile("");
    public static final Pattern GROUPING_PATTERN = Pattern.compile("");

    public static final HashMap<Token.Type, Pattern> PATTERN_TABLE = new HashMap<>() {{
        put(Token.Type.LITERAL, LITERAL_PATTERN);
        put(Token.Type.KEYWORD, KEYWORD_PATTERN);
        put(Token.Type.IDENTIFIER, IDENTIFIER_PATTERN);
        put(Token.Type.OPERATOR, OPERATOR_PATTERN);
        put(Token.Type.GROUPING, GROUPING_PATTERN);
    }};

    private final String path, source;
    private int position, line, column;

    public Lexer(String path, String source) {
        this.path = path;
        this.source = source;
        this.position = 0;
        this.line = this.column = 1;
    }

    public Token nextToken() {
        skipWhitespace();
        if (!isNotEof())
            return new Token(Token.Type.EOF, "", line, column);

        for (Map.Entry<Token.Type, Pattern> entry : PATTERN_TABLE.entrySet()) {
            Matcher matcher = entry.getValue().matcher(source);
            matcher.region(position, source.length());

            if (matcher.lookingAt()) {
                String lexeme = matcher.group();
                Token.Type type = entry.getKey();
                Token token = new Token(type, lexeme, line, column);
                consumeCharacters(lexeme.length());
                return token;
            }
        }

        char illegal = peekCharacter();
        consumeCharacters(1);

        // TODO: handle syntax error

        return null;
    }

    public LinkedList<Token> tokenize() {
        LinkedList<Token> tokens = new LinkedList<>();

        Token token;
        do {
            token = nextToken();
            if (token == null)
                return null;
            tokens.add(token);
        } while (token.type() != Token.Type.EOF);
        tokens.add(new Token(Token.Type.EOF, "\0", line, column));

        return tokens;
    }

    private boolean isNotEof() { return position < source.length(); }

    private char peekCharacter() { return isNotEof() ? source.charAt(position) : '\0'; }

    private void consumeCharacters(int count) {
        for (int i = 0; i < count; i++) {
            if (isNotEof()) {
                char current = peekCharacter();
                if (current == '\n') {
                    line++;
                    column = 0;
                } else if (current == '\t') {
                    column += 3;
                }
                column++;
                position++;
            }
        }
    }

    private void skipWhitespace() {
        while (isNotEof() && Character.isWhitespace(peekCharacter()))
            consumeCharacters(1);
    }

}