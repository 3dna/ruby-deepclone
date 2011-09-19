// Include the Ruby headers and goodies
#include "ruby.h"
#include "st.h"
#include "env.h"
#include "node.h"

// Defining a space for information and references about the module to be stored internally
VALUE DeepClone = Qnil;

// Prototype for the initialization method - Ruby calls this, not you
void Init_deepclone();

// Prototype for our method 'test1' - methods are prefixed by 'method_' here
VALUE method_clone(int argc,VALUE argv);

// The initialization method for this module
void Init_deepclone() {
	DeepClone = rb_define_module("DeepClone");
	rb_define_module_function(DeepClone, "clone", method_clone, 1);
}
static int clone_object(ID key, VALUE entry, VALUE new_obj)     
{  
   rb_ivar_set(new_obj, key, method_clone(0,entry));
   return ST_CONTINUE;
}

static int hash_each(VALUE key, VALUE value, VALUE new_hash)
{
   rb_hash_aset(new_hash,method_clone(0,key),method_clone(0,value));
   return ST_CONTINUE;
}

// Our 'test1' method.. it simply returns a value of '10' for now.
VALUE method_clone(int argc,VALUE argv)  
{	
    VALUE new_obj;   
    if(!rb_special_const_p(argv)){       
       switch (BUILTIN_TYPE(argv)) {
         case T_ARRAY:
            new_obj = rb_ary_new2(RARRAY(argv)->len);           
               long len = RARRAY(argv)->len;
               VALUE *ptr = RARRAY(argv)->ptr;               
               while (len--) {                                                               
                  rb_ary_push(new_obj,method_clone(0,*ptr));
                  ptr++;  
               }            
            break;
         case T_HASH:
            new_obj = rb_hash_new();
            rb_hash_foreach(argv, hash_each, new_obj);
            break;         
         case T_STRING:
            new_obj = rb_obj_clone(argv);            
            break;
         case T_CLASS:
            new_obj = argv;
            break;
         case T_MODULE:
            new_obj = argv;
            break;
         case T_REGEXP:
            new_obj = argv;
            break;
         default:
            new_obj = rb_obj_clone(argv);
            if (ROBJECT(argv)->iv_tbl) {
              st_foreach(ROBJECT(argv)->iv_tbl, clone_object, new_obj);
            }
            break;
        }
    }else{
       new_obj = argv;
    }       	
    return new_obj;
}
