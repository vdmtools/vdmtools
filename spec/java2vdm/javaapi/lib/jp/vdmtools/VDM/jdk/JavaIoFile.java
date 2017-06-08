

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
                    new Boolean(false));

    public JavaIoFile() {
    }

    public JavaIoFile(File f) {
        wrappedObject = f;
    }


    public HashMap list() throws CGException {
        HashMap result = new HashMap();
        String names[] = wrappedObject.list();
        for (int i = 0; i < names.length; i++)
            result.put(new Integer(i), new JavaLangString(names[i]));
        return result;
    }


    public Boolean mkdir() {
        return new Boolean(wrappedObject.mkdir());
    }


    public JavaNetURL toURL() throws CGException {
        return new JavaNetURL(new quotes.NIL());
    }


    public Boolean delete() {
        return new Boolean(wrappedObject.delete());
    }


    public Boolean isFile() {
        return new Boolean(wrappedObject.isFile());
    }


    public Integer length() {
        return new Integer((int) wrappedObject.length());
    }


    public Boolean mkdirs() {
        return new Boolean(wrappedObject.mkdirs());
    }


    public Boolean canRead() throws CGException {
        return new Boolean(wrappedObject.canRead());
    }


    public JavaLangString getName() throws CGException {
        return new JavaLangString(wrappedObject.getName());
    }


    public JavaLangString getPath() throws CGException {
        return new JavaLangString(wrappedObject.getPath());
    }


    public Boolean canWrite() throws CGException {
        return new Boolean(wrappedObject.canWrite());
    }


    public Boolean exists_u_u() throws CGException {
        return new Boolean(wrappedObject.exists());
    }


    public JavaLangClass getJClass() throws CGException {
        return CLASS;
    }


    public Integer jHashCode() throws CGException {
        return new Integer(wrappedObject.hashCode());
    }


    public Boolean isHidden() throws CGException {
        return new Boolean(wrappedObject.isHidden());
    }


    public JavaLangString toJString() throws CGException {
        return getPath();
    }

    public JavaIoFile(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = new Boolean(true);
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
            result.put(new Integer(i),
                    new JavaIoFile(getPath(),
                            (JavaLangString) ss.get(new Integer(i))));
        return result;
    }

    static public HashMap listRoots() throws CGException {
        return new HashMap();
    }


    public Boolean isAbsolute() {
        return new Boolean(wrappedObject.isAbsolute());
    }

    public Boolean isDirectory() {
        return new Boolean(wrappedObject.isDirectory());
    }

    public Boolean setReadOnly() {
        return new Boolean(wrappedObject.setReadOnly());
    }


    public JavaIoFile(final JavaLangString pathname) throws CGException {
        wrappedObject = new File(pathname.toSeqOfChar());
    }


    public void deleteOnExit() {
        wrappedObject.deleteOnExit();
    }


    public Integer lastModified() {
        return new Integer((int) wrappedObject.lastModified());
    }

    public Boolean createNewFile() throws CGException {
        try {
            return new Boolean(wrappedObject.createNewFile());
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
            return new Boolean(false);

        return new Boolean(wrappedObject.equals(((JavaIoFile) obj).getWrappedObject()));
    }


    public Boolean renameTo(final JavaIoFile dest) {
        return new Boolean(wrappedObject.renameTo(dest.getWrappedObject()));
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
        return new Boolean(wrappedObject.setLastModified(time.intValue()));
    }


    public Integer compareTo(final JavaLangObject pathname) throws CGException {
        return new Integer(wrappedObject.compareTo(((JavaIoFile) pathname).getWrappedObject()));
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

