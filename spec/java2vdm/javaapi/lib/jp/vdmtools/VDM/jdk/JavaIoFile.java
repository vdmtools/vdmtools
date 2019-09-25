

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.io.File;
import java.util.HashMap;


public class JavaIoFile extends JavaLangObject implements JavaIoSerializable, JavaLangComparable {

    private File wrappedObject;

    public static JavaLangCharacter separatorChar = new JavaLangCharacter(new Character(File.separatorChar));

    public static JavaLangString separator = new JavaLangString(File.separator);

    public static JavaLangCharacter pathSeparatorChar = new JavaLangCharacter(new Character(File.pathSeparatorChar));

    public static JavaLangString pathSeparator = new JavaLangString(File.pathSeparator);

    private static final JavaLangClass CLASS =
            new JavaLangClass(new String("JavaIoFile"),
                    new quotes.CLASS(),
                    Boolean.FALSE);

    public JavaIoFile() {
    }

    public JavaIoFile(File f) {
        wrappedObject = f;
    }


    public HashMap list() throws CGException {
        HashMap result = new HashMap();
        String names[] = wrappedObject.list();
        for (int i = 0; i < names.length; i++)
            result.put(Integer.valueOf(i), new JavaLangString(names[i]));
        return result;
    }


    public Boolean mkdir() {
        return Boolean.valueOf(wrappedObject.mkdir());
    }


    public JavaNetURL toURL() throws CGException {
        return new JavaNetURL(new quotes.NIL());
    }


    public Boolean delete() {
        return Boolean.valueOf(wrappedObject.delete());
    }


    public Boolean isFile() {
        return Boolean.valueOf(wrappedObject.isFile());
    }


    public Integer length() {
        return Integer.valueOf((int) wrappedObject.length());
    }


    public Boolean mkdirs() {
        return Boolean.valueOf(wrappedObject.mkdirs());
    }


    public Boolean canRead() throws CGException {
        return Boolean.valueOf(wrappedObject.canRead());
    }


    public JavaLangString getName() throws CGException {
        return new JavaLangString(wrappedObject.getName());
    }


    public JavaLangString getPath() throws CGException {
        return new JavaLangString(wrappedObject.getPath());
    }


    public Boolean canWrite() throws CGException {
        return Boolean.valueOf(wrappedObject.canWrite());
    }


    public Boolean exists_u_u() throws CGException {
        return Boolean.valueOf(wrappedObject.exists());
    }


    public JavaLangClass getJClass() throws CGException {
        return CLASS;
    }


    public Integer jHashCode() throws CGException {
        return Integer.valueOf(wrappedObject.hashCode());
    }


    public Boolean isHidden() throws CGException {
        return Boolean.valueOf(wrappedObject.isHidden());
    }


    public JavaLangString toJString() throws CGException {
        return getPath();
    }

    public JavaIoFile(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }


    public JavaLangString getParent() throws CGException {
        return new JavaLangString(wrappedObject.getParent());
    }

    public HashMap listFiles() throws CGException {
        HashMap ss = list();
        if (ss == null)
            return null;
        int n = ss.size();
        HashMap result = new HashMap();
        for (int i = 0; i < n; i++)
            result.put(Integer.valueOf(i),
                    new JavaIoFile(getPath(),
                            (JavaLangString) ss.get(Integer.valueOf(i))));
        return result;
    }

    static public HashMap listRoots() throws CGException {
        return new HashMap();
    }


    public Boolean isAbsolute() {
        return Boolean.valueOf(wrappedObject.isAbsolute());
    }

    public Boolean isDirectory() {
        return Boolean.valueOf(wrappedObject.isDirectory());
    }

    public Boolean setReadOnly() {
        return Boolean.valueOf(wrappedObject.setReadOnly());
    }


    public JavaIoFile(final JavaLangString pathname) throws CGException {
        wrappedObject = new File(pathname.toSeqOfChar());
    }


    public void deleteOnExit() {
        wrappedObject.deleteOnExit();
    }


    public Integer lastModified() {
        return Integer.valueOf((int) wrappedObject.lastModified());
    }

    public Boolean createNewFile() throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.createNewFile());
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public JavaIoFile getParentFile() throws CGException {
        JavaLangString p = this.getParent();
        if (p == null)
            return null;
        return new JavaIoFile(p);
    }

    public JavaIoFile(final JavaLangString pathname,
                      final Integer prefixLength_1) throws CGException {
        wrappedObject = new File(pathname.toSeqOfChar());
    }

    public JavaIoFile(final JavaIoFile parent,
                      final JavaLangString vdm_child) throws CGException {
        wrappedObject = new File(parent.getPath().toSeqOfChar(),
                vdm_child.toSeqOfChar());
    }


    public File getWrappedObject() {
        return wrappedObject;
    }

    public Boolean equals(final JavaLangObject obj) throws CGException {
        if ((obj == null) || (obj.IsNil().booleanValue())
                || (!(obj instanceof JavaIoFile)))
            return Boolean.FALSE;

        return Boolean.valueOf(wrappedObject.equals(((JavaIoFile) obj).getWrappedObject()));
    }


    public Boolean renameTo(final JavaIoFile dest) {
        return Boolean.valueOf(wrappedObject.renameTo(dest.getWrappedObject()));
    }

    public JavaIoFile getAbsoluteFile() throws CGException {
        return new JavaIoFile(getAbsolutePath());
    }

    public JavaLangString getAbsolutePath() throws CGException {
        return new JavaLangString(wrappedObject.getAbsolutePath());
    }

    public JavaIoFile getCanonicalFile() throws CGException {
        return new JavaIoFile(getCanonicalPath());
    }


    public JavaLangString getCanonicalPath() throws CGException {
        try {
            return new JavaLangString(wrappedObject.getCanonicalPath());
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }


    public Boolean setLastModified(final Integer time) {
        return Boolean.valueOf(wrappedObject.setLastModified(time.intValue()));
    }


    public Integer compareTo(final JavaLangObject pathname) throws CGException {
        return Integer.valueOf(wrappedObject.compareTo(((JavaIoFile) pathname).getWrappedObject()));
    }


    public JavaIoFile(final JavaLangString parent, final JavaLangString vdm_child) throws CGException {
        wrappedObject = new File(parent.toSeqOfChar(),
                vdm_child.toSeqOfChar());
    }


    static public JavaIoFile createTempFile(final JavaLangString prefix,
                                            final JavaLangString suffix)
            throws CGException {
        try {
            File file = File.createTempFile(prefix.toSeqOfChar(),
                    suffix.toSeqOfChar());
            return new JavaIoFile(new JavaLangString(file.getAbsolutePath()));
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }

    }

    static public JavaIoFile createTempFile(final JavaLangString prefix,
                                            final JavaLangString suffix,
                                            final JavaIoFile directory)
            throws CGException {
        try {
            File file = File.createTempFile(prefix.toSeqOfChar(),
                    suffix.toSeqOfChar(),
                    directory.getWrappedObject());
            return new JavaIoFile(new JavaLangString(file.getAbsolutePath()));
        } catch (java.io.IOException e) {
            throw new VDMSpecException(new JavaIoIOException());
        }
    }

}

