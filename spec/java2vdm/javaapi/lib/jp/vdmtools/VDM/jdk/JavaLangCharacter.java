package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;


public class JavaLangCharacter extends JavaLangObject {

    public static final Integer MIN_uRADIX = new Integer(Character.MIN_RADIX);
    public static final Integer MAX_uRADIX = new Integer(Character.MAX_RADIX);
    public static final Character MIN_uVALUE =
            new Character(Character.MIN_VALUE);
    public static final Character MAX_uVALUE =
            new Character(Character.MAX_VALUE);


    private Character wrappedObject;

    public Character getWrappedObject() {
        return wrappedObject;
    }

    public JavaLangCharacter() {
        wrappedObject = new Character('a');
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
            return new Integer(wrappedObject.compareTo(
                    ((JavaLangCharacter) o).getWrappedObject()));
        else
            throw new VDMSpecException(new JavaLangClassCastException());
    }


    public Boolean equals(final JavaLangObject o) throws CGException {
        return new Boolean(compareTo((JavaLangCharacter) o).intValue() == 0);
    }

    public Integer jHashCode() {
        return new Integer(wrappedObject.hashCode());
    }

    public JavaLangString toJString() throws CGException {
        return new JavaLangString(wrappedObject.toString());
    }

    public String toSeqOfChar() {
        return wrappedObject.toString();
    }

    static public Integer getType(final Character ch) {
        return new Integer(Character.getType(ch.charValue()));
    }

    static public Boolean isSpace(final Character ch) {
//        return new Boolean(Character.isSpace(ch.charValue()));
        return new Boolean(Character.isWhitespace(ch.charValue()));
    }


    static public Boolean isDefined(final Character ch) {
        return new Boolean(Character.isDefined(ch.charValue()));
    }


    static public Character forDigit(final Integer digit_1,
                                     final Integer radix) {
        return new Character(Character.forDigit(digit_1.intValue(),
                radix.intValue()));
    }


    static public Boolean isLowerCase(final Character ch) {
        return new Boolean(Character.isLowerCase(ch.charValue()));
    }


    static public Boolean isTitleCase(final Character ch) {
        return new Boolean(Character.isTitleCase(ch.charValue()));
    }


    static public Boolean isUpperCase(final Character ch) {
        return new Boolean(Character.isUpperCase(ch.charValue()));
    }


    static public Character toLowerCase(final Character ch) {
        return new Character(Character.toLowerCase(ch.charValue()));
    }


    static public Character toTitleCase(final Character ch) {
        return new Character(Character.toTitleCase(ch.charValue()));
    }


    static public Character toUpperCase(final Character ch) {
        return new Character(Character.toUpperCase(ch.charValue()));
    }


    static public Boolean isISOControl(final Character ch) {
        return new Boolean(Character.isISOControl(ch.charValue()));
    }


    static public Boolean isJavaLetter(final Character ch) {
//        return new Boolean(Character.isJavaLetter(ch.charValue()));
        return new Boolean(Character.isJavaIdentifierStart(ch.charValue()));
    }


    public JavaLangCharacter(final quotes.NIL n) {
        isNil = new Boolean(true);
    }


    static public Integer getNumericValue(final Character ch) {
        return new Integer(Character.getNumericValue(ch.charValue()));
    }


    static public Boolean isJavaLetterOrDigit(final Character ch) {
//        return new Boolean(Character.isJavaLetterOrDigit(ch.charValue()));
        return new Boolean(Character.isJavaIdentifierPart(ch.charValue()));
    }


    static public Boolean isJavaIdentifierPart(final Character ch) {
        return new Boolean(Character.isJavaIdentifierPart(ch.charValue()));
    }


    static public Boolean isIdentifierIgnorable(final Character ch) {
        return new Boolean(Character.isIdentifierIgnorable(ch.charValue()));
    }


    static public Boolean isJavaIdentifierStart(final Character ch) {
        return new Boolean(Character.isJavaIdentifierStart(ch.charValue()));
    }


    static public Boolean isUnicodeIdentifierPart(final Character ch) {
        return new Boolean(Character.isUnicodeIdentifierPart(ch.charValue()));
    }


    static public Boolean isUnicodeIdentifierStart(final Character ch) {
        return new Boolean(Character.isUnicodeIdentifierStart(ch.charValue()));
    }


    static public Integer digit(final Character ch, final Integer radix) {
        return new Integer(Character.digit(ch.charValue(), radix.intValue()));
    }


    static public Boolean isDigit(final Character ch) {
        return new Boolean(Character.isDigit(ch.charValue()));
    }


    static public Boolean isLetter(final Character ch) {
        return new Boolean(Character.isLetter(ch.charValue()));
    }


    static public Boolean isLetterOrDigit(final Character ch) {
        return new Boolean(Character.isLetterOrDigit(ch.charValue()));
    }


    static public Boolean isSpaceChar(final Character ch) {
        return new Boolean(Character.isSpaceChar(ch.charValue()));
    }


    static public Boolean isWhitespace(final Character ch) {
        return new Boolean(Character.isWhitespace(ch.charValue()));
    }
}

