goog.provide('rethinkdb.query');

/**
 * @name rethinkdb.errors
 * @namespace Contains error classes for ReQL errors
 */
goog.provide('rethinkdb.errors');

/**
 * This is defined here so that it is defined first, before
 * any thing defined on the rethinkdb.query namespace is.
 */

/**
 * A shortcut function for wrapping values with RethinkDB expressions.
 * @namespace namespace for all ReQL query generating functions
 * @export
 */
rethinkdb.query = function(jsobj) {
    if (typeof jsobj === 'string' && (jsobj[0] === '$' || jsobj[0] === '@')) {
        return rethinkdb.query.R(jsobj);
    } else {
        return rethinkdb.query.expr(jsobj);
    }
};

/**
 * An error generated by this client, not the rethinkdb server
 * @param {?string=} opt_msg The error message
 * @constructor
 * @extends {Error}
 */
rethinkdb.errors.ClientError = function(opt_msg) {
    this.name = "RDB Client Error";
    this.message = opt_msg || "The RDB client has experienced an error";
};
goog.inherits(rethinkdb.errors.ClientError, Error);

/**
 * Internal utility for wrapping API function arguments
 * @param {*} val The value to wrap
 * @returns rethinkdb.query.Expression
 * @ignore
 */
function wrapIf_(val) {
    if (val instanceof rethinkdb.query.Expression) {
        return val;
    } else {
        return rethinkdb.query(val);
    }
}

/**
 * Internal utility for wrapping API function arguments that
 * are expected to be function expressions.
 * @param {rethinkdb.query.FunctionExpression|function(...)} fun The function to wrap
 * @returns rethinkdb.query.FunctionExpression
 * @ignore
 */
function functionWrap_(fun) {
    if (fun instanceof rethinkdb.query.FunctionExpression) {
        // No wrap needed
    } else if (fun instanceof rethinkdb.query.Expression) {
        fun = rethinkdb.query.fn('', fun);
    } else if(typeof fun === 'function') {
        fun = rethinkdb.query.fn(fun);
    } else {
       throw TypeError("Argument expected to be a function expression");
    }

    return fun;
}
