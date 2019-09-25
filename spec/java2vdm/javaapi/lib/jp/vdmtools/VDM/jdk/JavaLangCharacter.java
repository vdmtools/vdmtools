package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;


public class JavaLangCharacter extends JavaLangObject {

    public static final Integer MIN_uRADIX = Integer.valueOf(Character.MIN_RADIX);
    public static final Integer MAX_uRADIX = Integer.valueOf(Character.MAX_RADIX);
    public static final Character MIN_uVALUE =
            Character.valueOf(Character.MIN_VALUE);
    public static final Character MAX_uVALUE =
            Character.valueOf(Character.MAX_VALUE);


    private Character wrappedObject;

    public Character getWrappedObject() {
        return wrappedObject;
    }

    public JavaLangCharacter() {
        wrappedObject = Character.valueOf('a');
    }

    public JavaLangCharacter(final Character ch) {
        wrappedObject = ch;
    }

    public Character charValue() {
        return wrappedObject;
    }

    public Integer compareTo(final JavaLangObject o) throws CGException {
        if (o.IsNil().booleanValue())
            throw new VDMSpecException(new JavaLangClassCastException());
        else if (o instanceof JavaLangCharacter)
            return Integer.valueOf(wrappedObject.compareTo(
                    ((JavaLangCharacter) o).getWrappedObject()));
        else
            throw new VDMSpecException(new JavaLangClassCastException());
    }


    public Boolean equals(final JavaLangObject o) throws CGException {
        return Boolean.valueOf(compareTo((JavaLangCharacter) o).intValue() == 0);
    }

    public Integer jHashCode() {
        return Integer.valueOf(wrappedObject.hashCode());
    }

    public JavaLangString toJString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }

    public String toSeqOfChar() {
        return wrappedObject.toString();
    }

    static public Integer getType(final Character ch) {
        return Integer.valueOf(Character.getType(ch.charValue()));
    }

    static public Boolean isSpace(final Character ch) {
//        return Boolean.valueOf(Character.isSpace(ch.charValue()));
        return Boolean.valueOf(Character.isWhitespace(ch.charValue()));
    }


    static public Boolean isDefined(final Character ch) {
        return Boolean.valueOf(Character.isDefined(ch.charValue()));
    }


    static public Character forDigit(final Integer digit_1,
                                     final Integer radix) {
        return Character.valueOf(Character.forDigit(digit_1.intValue(),
                radix.intValue()));
    }


    static public Boolean isLowerCase(final Character ch) {
        return Boolean.valueOf(Character.isLowerCase(ch.charValue()));
    }


    static public Boolean isTitleCase(final Character ch) {
        return Boolean.valueOf(Character.isTitleCase(ch.charValue()));
    }


    static public Boolean isUpperCase(final Character ch) {
        return Boolean.valueOf(Character.isUpperCase(ch.charValue()));
    }


    static public Character toLowerCase(final Character ch) {
        return Character.valueOf(Character.toLowerCase(ch.charValue()));
    }


    static public Character toTitleCase(final Character ch) {
        return Character.valueOf(Character.toTitleCase(ch.charValue()));
    }


    static public Character toUpperCase(final Character ch) {
        return Character.valueOf(Character.toUpperCase(ch.charValue()));
    }


    static public Boolean isISOControl(final Character ch) {
        return Boolean.valueOf(Character.isISOControl(ch.charValue()));
    }


    static public Boolean isJavaLetter(final Character ch) {
//        return Boolean.valueOf(Character.isJavaLetter(ch.charValue()));
        return Boolean.valueOf(Character.isJavaIdentifierStart(ch.charValue()));
    }


    public JavaLangCharacter(final quotes.NIL n) {
        isNil = Boolean.TRUE;
    }


    static public Integer getNumericValue(final Character ch) {
        return Integer.valueOf(Character.getNumericValue(ch.charValue()));
    }


    static public Boolean isJavaLetterOrDigit(final Character ch) {
//        return Boolean.valueOf(Character.isJavaLetterOrDigit(ch.charValue()));
        return Boolean.valueOf(Character.isJavaIdentifierPart(ch.charValue()));
    }


    static public Boolean isJavaIdentifierPart(final Character ch) {
        return Boolean.valueOf(Character.isJavaIdentifierPart(ch.charValue()));
    }


    static public Boolean isIdentifierIgnorable(final Character ch) {
        return Boolean.valueOf(Character.isIdentifierIgnorable(ch.charValue()));
    }


    static public Boolean isJavaIdentifierStart(final Character ch) {
        return Boolean.valueOf(Character.isJavaIdentifierStart(ch.charValue()));
    }


    static public Boolean isUnicodeIdentifierPart(final Character ch) {
        return Boolean.valueOf(Character.isUnicodeIdentifierPart(ch.charValue()));
    }


    static public Boolean isUnicodeIdentifierStart(final Character ch) {
        return Boolean.valueOf(Character.isUnicodeIdentifierStart(ch.charValue()));
    }


    static public Integer digit(final Character ch, final Integer radix) {
        return Integer.valueOf(Character.digit(ch.charValue(), radix.intValue()));
    }


    static public Boolean isDigit(final Character ch) {
        return Boolean.valueOf(Character.isDigit(ch.charValue()));
    }


    static public Boolean isLetter(final Character ch) {
        return Boolean.valueOf(Character.isLetter(ch.charValue()));
    }


    static public Boolean isLetterOrDigit(final Character ch) {
        return Boolean.valueOf(Character.isLetterOrDigit(ch.charValue()));
    }


    static public Boolean isSpaceChar(final Character ch) {
        return Boolean.valueOf(Character.isSpaceChar(ch.charValue()));
    }


    static public Boolean isWhitespace(final Character ch) {
        return Boolean.valueOf(Character.isWhitespace(ch.charValue()));
    }
}

