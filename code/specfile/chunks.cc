/***
*  * WHAT
*  *   Representation of a section in a specification document 
*  *   in terms of chunks, e.g. a VDM chunk (vdm_al in Latex)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/chunks.cc,v $
*  * VERSION
*  *    $Revision: 1.2 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:54 $
*  * STATUS
*  *    
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Lars T. Olsen + $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

bool Chunk::write_string_to_file(const wstring & s, ostream & o) {
  o << s;
}

// VDMChunk

// write the VDM chunk

bool VDMChunk::write_chunk() {
  wstring buf;
  wchar_t tmpbuf[100];
  sprintf(tmpbuf,L"\n--LINE %d %d",section_count,section);

  buf += tmpbuf;

    if (! first_chunk) buf += L"\n";
    if (reset_paragraph) buf += L"\\pard\\plain ";  

    if ( !first_chunk ) {
      chunk += this_style;
      chunk += chunk_buffer;
    } else {
      chunk += chunk_buffer;
    }

    chunks->ImpAppend((Token)chunk);

}
