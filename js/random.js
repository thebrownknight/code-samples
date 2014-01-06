//===========================================
// Example 1: Constructor function
//===========================================

// Function that performs the same process as creating a new constructor (but without the 'new' keyword)
// Takes in three parameters - the object to inherit from, the initializer function, and any attached methods
// Usage: var big_person = inheritFrom(person, function(name) { this.name = name; }, { speak: function(name){console.log("My name is " + name);} })
function inheritFrom(extend, initializer, methods) {
	var func, prototype = Object.create(extend && extend.prototype);
	// if methods are available
	if (methods) {
		// keys() method returns an array of all of the object's own keys - new in ES5
		// use array's forEach method to copy all of the methods into the prototype
		methods.keys().forEach(function (key) {
			prototype[key] = methods[key];
		});
	}
	// create the function constructor itself
	func = function () {
		// Create a new instance of the prototype
		// makes new object which inherits from the object you pass in
		var that = Object.create(prototype);
		// If the initializer is actually a function, apply the 
		// arguments of the function to the object we just created
		if (typeof initializer === 'function') {
			initializer.apply(that, arguments);
		}
		return that;	// return the object we just created
	};
	func.prototype = prototype;	// explicitly set the prototype of the function to the prototype
	prototype.constructor = func;	// replace the lost constructor on the prototype
	return func;	// return the new constructor function
}



//===========================================
// Example 2: Module pattern example
//===========================================
String.prototype.replaceEntity = (function() {
	// Entity table - maps entity names to characters
	// Private variable, only accessible by the replaceEntity method
	var entity = {
		quot: '"',
		lt:   '<',
		gt:   '>'
	};
	// Return the replaceEntity method
	return function () {
		// Calls the string replace method, looking for 
		// substrings that start with '&' and end with ';'.

		// If the characters between are in the entity table,
		// then replace the entity with the character from
		// the table.

		// E.g. &quot; will be replaced with a ".
		return this.replace(/&[^&;];/g, function(a, b) {
			var r = entity[b];
			return typeof r === 'string' ? r : a;
		});
	};
}());

//===========================================
// Example 3: JavaScript recursion example
//===========================================

// Method that traverses the DOM in HTML source order
// Params: the node we start at, the function to execute on the node
// Notice it is named twice - for easier access in recursion
var walkDOM = function walk(curNode, pfunc) {
	// Call the function on the current curNode
	pfunc(curNode);
	// Set the curNode to it's first child (will be null if there is none)
	curNode = curNode.firstChild;
	// The recursion continues till there aren't any more children,
	// and then it looks for siblings
	while (curNode) {
		walk(curNode, pfunc);
		curNode = curNode.nextSibling;
	}
}

// The above method can be used in conjunction with
// retrieving DOM elements based on attributes, for example
// Params: attribute name, optional matching value
var getElementsByAttribute = function (attr, value) {
	var res_elems = [];	// array to contain the elements

	// call the recursive walk method starting at the body element
	walkDOM(document.body, function(node) {
		// Try to get the attribute with the given name from the node
		// if it is an element node
		// node.getAttribute returns the VALUE of the node's attribute
		var actual = node.nodeType === 1 && node.getAttribute(attr);
		// Check to make sure we are returned a string (and not null)
		// Then check to see if the attribute's value strict equals
		// the value (if it is given as a parameter), or ignore if it isn't a string
		if (typeof actual === 'string' &&
			(actual === value || typeof value !== 'string')) {
			res_elems.push(node);		// add the node to the result elements array
		}
	});
}