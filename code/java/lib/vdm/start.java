package jp.vdmtools.VDM.quotes;

public class start
{

    static private int hc = 0;

    public start()
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
        return obj instanceof start;
    }

    public String toString()
    {
        return "<start>";
    }
}
