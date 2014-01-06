/* Utility function to create the notifications */
function create( template, vars, opts ){
    return $container.notify("create", template, vars, opts);
}

/* Error object that can be used to display all errors. */
function SNError(type, message, targetElement, targetMsgElement, append) {
    this.type = type;
    this.message = message;
    this.targetElement = targetElement;
    this.targetMsgElement = targetMsgElement;
    this.append = append;

    this.displayError = function(){
        if(targetElement != null) {
            targetElement.css( {'border-color':'#BA0909', 'box-shadow':'inset 0 0 0.3em #BA0909', '-moz-box-shadow':'inset 0 0 0.3em #BA0909', '-webkit-box-shadow':'inset 0 0 0.3em #BA0909'} );  
        }
        if(append === 0)
            targetMsgElement.empty().text(this.message);
        else if(append === 1)
            targetMsgElement.append(this.message);
    };
    this.setMessage = function(new_msg){
        this.message = new_msg;
    };
    this.setTargetElement = function(new_targetElement){
        this.targetElement = new_targetElement;
    }
    this.setTargetMsgElement = function(new_targetMsgElement){
        this.targetMsgElement = new_targetMsgElement;
    }
    this.setAppend = function(new_append){
        this.append = new_append;
    }
}

////////////////////////////////////////////////
//  FUNCTION: sendAjax
//  DESCRIPTION: a wrapper function around sending
//  simple AJAX calls 
//  PARAMS: ajaxParams object with type, url, 
//  data, dataType, done callback, failure callback
////////////////////////////////////////////////
function sendAjax(type, url, data, dataType, doneCB, failCB) {
    $.ajax({
        type: type,
        url: url,
        data: data,
        dataType: dataType,
    })
    .done(doneCB)
    .fail(failCB);
}

////////////////////////////////////////////////
//  FUNCTION: populateDropdown
//  DESCRIPTION: takes a selector for a select
//  element and fills it with an array of content
//  PARAMS: selector, content array, options object
////////////////////////////////////////////////
function populateDropdown(selector, content, options) {
        newListSelector = '<select id="' + selector + '">';    //create a new select to replace the old one
        var index = -1;

        if (options.hasOwnProperty('selected')) {
            if (typeof options['selected'] === "number") {
                index = options['selected'];
            }
            else if (typeof options['selected'] === "string"){
                if (options['selected'] !== 'none')
                    index = $.inArray(options['selected'], content);
                else
                    index = -2; // temporary value for 'none' being selected
            }
        }

        newListSelector += '<option selected="selected" value="none">--None--</option>';

        for (var i=0, listLen = content.length; i < listLen; ++i){
            if (index != -1 && i == index)   
                newListSelector += '<option selected="selected" value="' + content[i] + '">' + content[i] + '</option>';
            else if (i != index)
                newListSelector += '<option value="' + content[i] + '">' + content[i] + '</option>';
        }

        newListSelector += '</select>';

        $('#' + selector).replaceWith(newListSelector);  //replace the old select control with the newly constructed one

        // Bind change handler
        if (options.hasOwnProperty('onchange')) {
            $('#' + selector).on('change', options['onchange']);
        }

        console.log("Finished populating dropdown.");
}

// Function to merge two arrays uniquely
function arrayUnique(array) {
    var a = array.concat();
    for(var i=0; i<a.length; ++i) {
        for(var j=i+1; j<a.length; ++j) {
            if(a[i] === a[j])
                a.splice(j--, 1);
        }
    }
    return a;
}

// Function to return the keys of an array
function arrayKeys(input) {
    var output = new Array();
    var counter = 0;
    for (i in input) {
        output[counter++] = i;
    } 
    return output; 
}

// Function to return the count of an element in an array
function elemCount(inputAr, elem) {
    var counter = 0;
    for(var i = 0; i < inputAr.length; ++i){
        if (inputAr[i] === elem)
            counter++;
    }

    return counter;
}

// Function to generate random color
function get_random_color() {
    cur_color_list = [];
    /* One way of getting random colors
        var letters = '0123456789ABCDEF'.split('');
        var color = '#';
        for (var i = 0; i < 6; i++ ) {
            color += letters[Math.round(Math.random() * 15)];
        }
        return color;
    */
    /* Second way of getting colors. */
    // var max = 0xffffff;
    // return '#' + Math.round( Math.random() * max ).toString( 16 );

    /* Third way - have a predetermined array of colors to use. */
    var colors = [  "#000000", "#2C3539", "#2B1B17", "#34282C", "#25383C", "#3B3131", "#413839", "#463E3F", "#4C4646",
                    "#2B547E", "#2B3856", "#151B54", "#000080", "#342D7E", "#15317E", "#151B8D", "#0000A0", "#0020C2",
                    "#FF0000", "#FF2400", "#F62217", "#F70D1A", "#F62817", "#E42217", "#E41B17", "#DC381F", "#C34A2C",
                    "#C24641", "#C04000", "#C11B17", "#9F000F", "#990012", "#8C001A", "#7E3517", "#8A4117", "#7E3817",
                    "#800517", "#810541", "#7D0541", "#7E354D", "#7D0552", "#3EA99F", "#3B9C9C", "#438D80", "#348781",
                    "#307D7E", "#008080", "#617C58", "#728C00", "#667C26", "#254117", "#306754", "#347235", "#437C17",
                    "#387C44", "#347C2C", "#347C17", "#348017", "#6AA121", "#4AA02C", "#41A317", "#3EA055", "#6CBB3C", "#00FF00"];
    var chosenColor = colors[Math.round( Math.random() * (colors.length - 1))];
    // Prevent the same color being randomly chosen twice
    if($.inArray(chosenColor, cur_color_list) >= 0)
        chosenColor = colors[Math.round( Math.random() * (colors.length - 1))];

    cur_color_list.push(chosenColor);
    return chosenColor;
}