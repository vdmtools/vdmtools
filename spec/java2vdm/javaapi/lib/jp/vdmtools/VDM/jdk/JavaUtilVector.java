/*
 * Created by IntelliJ IDEA.
 * User: paulm
 * Date: Nov 22, 2001
 * Time: 7:06:32 PM
 * To change template for new class use 
 * Code Style | Class Templates options (Tools | IDE Options).
 */
package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.*;

public class JavaUtilVector extends JavaLangObject implements JavaUtilList {
    private Vector wrappedObject;

    public Vector getWrappedObject()
    {
        return wrappedObject;
    }

    public Integer vdm_hashCode()
    {
	return Integer.valueOf(wrappedObject.hashCode());
    }

    public Boolean vdm_equals(final JavaLangObject o) throws CGException
    {
	if (!(o instanceof JavaUtilVector))
	    return Boolean.FALSE;

	return Boolean.valueOf(wrappedObject.equals(((JavaUtilVector) o).getWrappedObject()));
    }

    public JavaUtilVector(quotes.NIL v){
        isNil = Boolean.TRUE;
    }

    public JavaUtilVector(){
        wrappedObject = new Vector();
    }

    public JavaUtilVector(Vector v)
    {
        wrappedObject = v;
    }

    public JavaUtilVector(Integer i){
        wrappedObject = new Vector(i.intValue());
    }

    public JavaUtilVector(Integer initialCapacity, Integer capacityIncrement)
    {
        wrappedObject = new Vector(initialCapacity.intValue(),
                                   capacityIncrement.intValue());
    }

    public Boolean add(JavaLangObject o)
    {
        return Boolean.valueOf(wrappedObject.add(o));
    }

    public Boolean addAll(JavaUtilCollection c) throws CGException
    {
        if (c.size().intValue() == 0)
            return Boolean.TRUE;

        JavaUtilIterator it = c.iterator();
        while (it.hasNext().booleanValue())
            add(it.next());

        return Boolean.TRUE;
    }


    public Boolean addAll(Integer index, JavaUtilCollection c) throws CGException
    {
	return addAllIn(index, c);
    }

    public Boolean addAllIn(Integer index, JavaUtilCollection c)
    throws CGException
    {
        if ((index.intValue() > 0) || (index.intValue() > size().intValue()))
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());

        if (index.intValue() == 0)
            return Boolean.TRUE;

        return Boolean.valueOf(wrappedObject.addAll(index.intValue(), convertCollection(c)));
    }

    private Collection convertCollection(JavaUtilCollection c) throws CGException
    {
        Vector v = new Vector();
        JavaUtilIterator it = c.iterator();
        while(it.hasNext().booleanValue())
            v.add(it.next());
        return v;
    }

    public void addElement(JavaLangObject o)
    {
        wrappedObject.add(o);
    }

    public Boolean addIntObj(Integer pos, JavaLangObject o)
      throws CGException
    {
        if ((pos.intValue() < 0) || (pos.intValue() > size().intValue()))
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());
        wrappedObject.add(pos.intValue(), o);
        return Boolean.TRUE;
    }

    public void insertElementAt(JavaLangObject obj, Integer index)
      throws CGException
    {
        try {
            wrappedObject.insertElementAt(obj, index.intValue());
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());
        }
    }

    class VectorIterator implements JavaUtilIterator
    {
	Iterator itWrappedObject;

	public Boolean IsNil()
	{
	    return Boolean.FALSE;
	}

	public VectorIterator(Iterator it)
	{
	    itWrappedObject = it;
	}

	public JavaLangObject next() throws CGException
	{
	    return (JavaLangObject) itWrappedObject.next();
	}

	public void remove() throws CGException
	{
	    itWrappedObject.remove();
	}

	public Boolean hasNext() throws CGException
	{
	    return Boolean.valueOf(itWrappedObject.hasNext());
	}
	
    }

    public JavaUtilIterator iterator()
    {
	return new VectorIterator(wrappedObject.iterator());
    }

    public void clear()
    {
        wrappedObject.clear();
    }

    public Object clone()
    {
        return new JavaUtilVector(wrappedObject);
    }

    public HashMap toArray(final HashMap a)
    {
	return toArray();
    }

    public HashMap toArray()
    {
        HashMap result = new HashMap();
        int index = 0;
        Iterator it = wrappedObject.iterator();
        while (it.hasNext()){
            result.put(Integer.valueOf(index), it.next());
            index++;
        }
        return result;
    }

    public Boolean contains(JavaLangObject elem)
    {
        return Boolean.valueOf(wrappedObject.contains(elem));
    }

    public Boolean containsAll(JavaUtilCollection c) throws CGException
    {
        return Boolean.valueOf(wrappedObject.containsAll(convertCollection(c)));
    }

    public JavaLangObject elementAt(Integer index) throws CGException
    {
        try {
            return (JavaLangObject) wrappedObject.elementAt(index.intValue());
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());
        }
    }

    public JavaLangObject firstElement() throws CGException
    {
        try  {
            return (JavaLangObject) wrappedObject.firstElement();
        } catch (NoSuchElementException e) {
            throw new VDMSpecException(new JavaUtilNoSuchElementException());
        }
    }

    public JavaLangObject get(Integer index) throws CGException
    {
        return elementAt(index);
    }

    public Boolean isEmpty()
    {
        return Boolean.valueOf(wrappedObject.isEmpty());
    }

    public Integer indexOf(JavaLangObject elem)
    {
        return Integer.valueOf(wrappedObject.indexOf(elem));
    }

    public Integer indexOfFrom(JavaLangObject elem, Integer index) throws CGException
    {
        try {
            return Integer.valueOf(wrappedObject.indexOf(elem, index.intValue()));
        } catch (IndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangIndexOutOfBoundsException());
        }
    }

    public JavaLangObject lastElement() throws CGException
    {
        try {
            return (JavaLangObject) wrappedObject.lastElement();
        } catch (NoSuchElementException e) {
            throw new VDMSpecException(new JavaUtilNoSuchElementException());
        }
    }

    public Integer lastIndexOf(JavaLangObject elem) throws CGException
    {
        return Integer.valueOf(wrappedObject.lastIndexOf(elem));
    }

    public JavaLangObject remove(Integer index) throws CGException
    {
        try {
            return (JavaLangObject) wrappedObject.remove(index.intValue());
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException ());
        }
    }

    public Boolean remove_q(JavaLangObject o)
    {
        return Boolean.valueOf(wrappedObject.remove(o));
    }

    public void removeAllElements()
    {
        wrappedObject.removeAllElements();
    }

    public void removeElementAt(Integer index) throws CGException
    {
        try {
            wrappedObject.removeElementAt(index.intValue());
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException (new JavaLangArrayIndexOutOfBoundsException());
        }
    }

    public Boolean removeElement(JavaLangObject obj)
    {
        return Boolean.valueOf(wrappedObject.removeElement(obj));
    }

    public Boolean removeAll(JavaUtilCollection c) throws CGException
    {
        return Boolean.valueOf(wrappedObject.removeAll(convertCollection(c)));
    }

    public Boolean retainAll(JavaUtilCollection c) throws CGException
    {
        return Boolean.valueOf(wrappedObject.retainAll(convertCollection(c)));
    }

    public JavaLangObject set_u_u(final Integer index, final JavaLangObject element) throws CGException
    {
	return set_q_q(index, element);
    }

    public JavaLangObject set_q_q(Integer index, JavaLangObject element) throws CGException
    {
        try {
            return (JavaLangObject) wrappedObject.set(index.intValue(), element);
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());
        } catch (IllegalArgumentException e) {
            throw new VDMSpecException(new JavaLangIllegalArgumentException());
        }
    }

    public void setElementAt(JavaLangObject element, Integer index) throws CGException
    {
        try {
            wrappedObject.setElementAt(element, index.intValue());
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());
        }
    }

    public Integer size()
    {
        return Integer.valueOf(wrappedObject.size());
    }

    public JavaUtilList subList(Integer fromIndex, Integer toIndex) throws CGException
    {
        try {
            List l = wrappedObject.subList(fromIndex.intValue(), toIndex.intValue());
            JavaUtilVector result = new JavaUtilVector();
            Iterator it = l.iterator();
            while(it.hasNext())
                result.add((JavaLangObject) it.next());
            return (JavaUtilList) result;
        } catch (ArrayIndexOutOfBoundsException e) {
            throw new VDMSpecException(new JavaLangArrayIndexOutOfBoundsException());
        } catch (IllegalArgumentException e) {
            throw new VDMSpecException(new JavaLangIllegalArgumentException());
        }
    }

    public JavaUtilEnumeration elements() throws CGException
    {
        return new VEnumeration(this);
    }

}
