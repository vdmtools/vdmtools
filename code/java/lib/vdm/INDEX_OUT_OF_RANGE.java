package jp.vdmtools.VDM;

/**
 * Index out of range in tuple or sequence.
 */

public class INDEX_OUT_OF_RANGE extends VDMRunTimeException {

  public INDEX_OUT_OF_RANGE() {
    super("Index out of range.");
  }

  public INDEX_OUT_OF_RANGE(String message) {
    super(message + ": Index out of range.");
  }
}
