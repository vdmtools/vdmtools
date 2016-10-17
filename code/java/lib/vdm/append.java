package jp.vdmtools.VDM.quotes;

public class append
{

    static private int hc = 0;

    public append()
    {
        if (hc == 0)
            hc = super.hashCode();
    }

    public int hashCode()
    {
        return hc;
    }

    public boolean equals(Object obj)
    {
        return obj instanceof append;
    }

    public String toString()
    {
        return "<append>";
    }
}
