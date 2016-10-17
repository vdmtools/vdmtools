mk_( <OK>,
     [ mk_ERR`ErrMsg( 
          "Warning : The function 'Test' is not used at all in module A",
          <WARNING> ),
       mk_ERR`ErrMsg( 
          "Warning : The function 'PMOD`f' is not used at all in module A",
          <WARNING> ),
       mk_ERR`ErrMsg( 
          "Warning : The constant value 'v' is not used at all in module A",
          <WARNING> ) ] )
