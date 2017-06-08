package jp.vdmtools.VDM.jdk;

public interface JavaUtilObserver extends Nullable {

    abstract public void update(final JavaUtilObservable o,
                                final JavaLangObject arg);

}

