/***
*  * WHAT
*  *   Representation of a section in a specification document 
*  *   in terms of chunks, e.g. a VDM chunk (vdm_al in Latex)
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/chunks.h,v $
*  * VERSION
*  *    $Revision: 1.6 $
*  * DATE
*  *    $Date: 1998/09/25 11:56:09 $
*  * STATUS
*  *    
*  * REFERENCES
*  *    
*  * PROJECT
*  *    Toolbox
*  * AUTHOR
*  *    Lars T. Olsen + $Author: jeppe $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <vector>

////////////////////////////////////////////
// This file defines the chunks that a SpecFile on an abstract level
// contains. The following kinds of chunks are supported:
// o VDM  - containg VDM in plain ASCII
// o BLOB - this chunk contains text 
// 
////////////////////////////////////////////

enum SpecFileChunkType {
  CHUNK_TYPE_FIRST = 0,

  CHUNK_TYPE_BLOB,        // Chunk containing text that we do not care about  
  CHUNK_TYPE_VDM,         // A chunk containing VDM in ascii format
  CHUNK_TYPE_VDM_TC_TABLE,// Module/class name where a tc table should be inserted 
  CHUNK_TYPE_VDM_IF,      // #if preprocessor chunk
  CHUNK_TYPE_VDM_ENDIF,   // #endif preprocessor chunk   
  CHUNK_TYPE_VDM_DEFINE,  // #define preprocessor chunk  

  CHUNK_TYPE_LAST,

  CHUNK_TYPE_ERROR        // Error of some kind 
};

////////////////////////////////////////////
// The class Chunk is the baseclass for all chunks
////////////////////////////////////////////

class Chunk {
private:
  int section;  // the section number of this chunk in a list of chunks
                // the first legal section number is 1
protected:
  SpecFileChunkType chunk_type;

public :

  string header;  // text that should be prefixed a chunk when pp
  string footer;  // text that should be appended to a chunk when pp

  Chunk() {
    chunk_type = CHUNK_TYPE_ERROR;
    section = 0;
  }

  SpecFileChunkType get_chunk_type() const { return chunk_type ;};

  // Get the section number for the chunk
  int get_section() const { return section ;  };
  void set_section(int s) { section = s ;};

  // Destructor in baseclass must be virtual (default is not)
  // (ref. to par. 12.4 in the c++ ref. manual)
  virtual ~Chunk() {}
};

////////////////////////////////////////////
// ChunkVDM
// This chunk contains VDM in plain ASCII
// that can bee feed to the parser
// The header and footer might contain 
// stuff that should be printed in case of 
// pretty printing the chunk.
////////////////////////////////////////////

class ChunkVDM : public Chunk {
  public :

  ChunkVDM() {
    chunk_type = CHUNK_TYPE_VDM;
  };
  string vdm_ascii;  // plain VDM ascii
};


////////////////////////////////////////////
// ChunkVDM_TC_TABLE
// This chunk contains the name of a module/class 
// for which a test coverage table should be printed
////////////////////////////////////////////

class ChunkVDM_TC_TABLE : public Chunk {
  public :

  ChunkVDM_TC_TABLE() {
    chunk_type = CHUNK_TYPE_VDM_TC_TABLE;
  };
  string mod_name;  // name of module/class
};

////////////////////////////////////////////
// ChunkBlob
// This chunk contains text
// 
////////////////////////////////////////////

class ChunkBLOB : public Chunk {
  public :    

  ChunkBLOB() {
    chunk_type = CHUNK_TYPE_BLOB;
  };

    string blob;
};

///////////////////////////////////////////////
// ChunkList
// List to hold Chunks from a specification
///////////////////////////////////////////////

class ChunkList {
  vector<Chunk*>  chunks;  // The list of Chunks
public :

    // Clean                                
    void Clean() 
    {
      for (unsigned int i = 0; i < chunks.size() ; i++ )
        if ( chunks[i] != NULL ) delete chunks[i];
      chunks.erase(chunks.begin(), chunks.end()); 
    }

    ~ChunkList() {
      Clean();
    };

    // Append Chunk to the list
    void Add(Chunk * c ) { 
        chunks.push_back(c);
    };

    // Get number of Chunks in list
    int Length() const {
      return chunks.size();
    };

    // Get Chunk with given index starting from 1
    const Chunk * Get(int i) const {
      return chunks[i-1];  
    };
};
