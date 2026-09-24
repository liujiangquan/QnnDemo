//! Thin JNI shim over the HuggingFace `tokenizers` crate.
//!
//! Exposes exactly what `com.breeze.qnn.laya.LayaTokenizer` needs:
//!   - nativeNew(path)          -> handle (0 on failure)
//!   - nativeEncode(handle, s)  -> int[] token ids (no special tokens), null on failure
//!   - nativeTokenToId(h, tok)  -> int id, or -1
//!   - nativeFree(handle)
//!
//! The Tokenizer is heap-allocated and its raw pointer handed to the JVM as a jlong handle.

use jni::objects::{JClass, JString};
use jni::sys::{jint, jintArray, jlong};
use jni::JNIEnv;
use tokenizers::Tokenizer;

/// `path` -> boxed Tokenizer pointer as jlong (0 on error). No special tokens are added at encode time.
#[no_mangle]
pub extern "system" fn Java_com_breeze_qnn_laya_LayaTokenizer_nativeNew(
    mut env: JNIEnv,
    _class: JClass,
    path: JString,
) -> jlong {
    let p: String = match env.get_string(&path) {
        Ok(s) => s.into(),
        Err(_) => return 0,
    };
    match Tokenizer::from_file(&p) {
        Ok(tk) => Box::into_raw(Box::new(tk)) as jlong,
        Err(_) => 0,
    }
}

/// Encode text -> token ids (add_special_tokens = false). Returns null on error.
#[no_mangle]
pub extern "system" fn Java_com_breeze_qnn_laya_LayaTokenizer_nativeEncode(
    mut env: JNIEnv,
    _class: JClass,
    handle: jlong,
    text: JString,
) -> jintArray {
    if handle == 0 {
        return std::ptr::null_mut();
    }
    let tk = unsafe { &*(handle as *const Tokenizer) };
    let s: String = match env.get_string(&text) {
        Ok(s) => s.into(),
        Err(_) => return std::ptr::null_mut(),
    };
    let enc = match tk.encode(s, false) {
        Ok(e) => e,
        Err(_) => return std::ptr::null_mut(),
    };
    let ids: Vec<jint> = enc.get_ids().iter().map(|&x| x as jint).collect();
    let arr = match env.new_int_array(ids.len() as jint) {
        Ok(a) => a,
        Err(_) => return std::ptr::null_mut(),
    };
    if env.set_int_array_region(&arr, 0, &ids).is_err() {
        return std::ptr::null_mut();
    }
    arr.into_raw()
}

/// Resolve a literal token (e.g. "<bos>") to its id, or -1 if unknown.
#[no_mangle]
pub extern "system" fn Java_com_breeze_qnn_laya_LayaTokenizer_nativeTokenToId(
    mut env: JNIEnv,
    _class: JClass,
    handle: jlong,
    token: JString,
) -> jint {
    if handle == 0 {
        return -1;
    }
    let tk = unsafe { &*(handle as *const Tokenizer) };
    let t: String = match env.get_string(&token) {
        Ok(s) => s.into(),
        Err(_) => return -1,
    };
    match tk.token_to_id(&t) {
        Some(id) => id as jint,
        None => -1,
    }
}

/// Free the Tokenizer behind the handle.
#[no_mangle]
pub extern "system" fn Java_com_breeze_qnn_laya_LayaTokenizer_nativeFree(
    _env: JNIEnv,
    _class: JClass,
    handle: jlong,
) {
    if handle != 0 {
        unsafe {
            drop(Box::from_raw(handle as *mut Tokenizer));
        }
    }
}
