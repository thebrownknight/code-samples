/************************************************************
* Author: Nikhil Venkatesh
* Date: 06/25/13
* Description: SNeT v0.2 extension to the AstroShelf project
*
* 10/21/2013 - Now on SNeT v0.3
* * Revised by: Nikhil Venkatesh
* Date: 10/21/2013
* Description: Change the layout of View Plan page
************************************************************/

/******************************Revision by Nikhil Begin ****************/

var dateList = new Array();
var NightLength = new Object();
var planTable;

function Comparator(a,b){
	if (a[1] < b[1]) return -1;
	if (a[1] > b[1]) return 1;
	return 0;
}

//TODO
function recalculate(item) {
			var txt = $(item).closest('td').find('span');

			var colid = $(item).closest('td').index();
			var value = parseInt($(item).val());
			var remainTd = $(item).closest('table').find("tr:last td:eq("+colid+")");
			var scheduleTd = $(item).closest('table').find("tr:last").prev().find("td:eq("+colid+")");

			var index = colid - 3;
			var id = "#dateHeader"+index;
			var headerSpan = $(id);

			//Observation times calculation
			var freqTd = $(item).closest('tr').find("td:last");
			var freqSpan = freqTd.find("span");
			var freqValue = parseInt(freqSpan.html().split("/")[0]); //selected value
			var specValue = parseInt(freqTd.find("input").val()); //User specified value

			//Supernova name (first td)
			var snSpan = $(item).closest('tr').find("td:first").find("span");

			var sValue = parseInt(scheduleTd.find("span").html());
			var color = scheduleTd.find("span").css("color");
			var rValue = parseInt(remainTd.find("span").html());
			var sHtml = 0;
			var rHtml = 0;

			if ($(item).is(':checked')) {
				txt.css("color","blue");
			//	var date = dateList[colid-3];
				freqValue = freqValue + 1;
				sHtml = sValue+value;
				rHtml = rValue-value;
			} else {
				txt.css("color","#D0D0D0");
				freqValue = freqValue - 1;
				sHtml = sValue-value;
				rHtml = rValue+value;
			}
			freqSpan.html(freqValue+"/"+specValue);
			if(freqValue<specValue){
				freqSpan.css("color","red");
				snSpan.css("color","red");
			}else{
				freqSpan.css("color","");
				snSpan.css("color","");
			}


			if(rHtml<0){
				headerSpan.css("color","red");
				scheduleTd.html("<span style='color:red;margin-left: 25px;font-weight:bold'>"+sHtml+"</span>");
				remainTd.html("<span style='color:red;margin-left: 25px;font-weight:bold'>"+rHtml+"</span>");
			}else{
				headerSpan.css("color","");
				scheduleTd.html("<span style='margin-left: 25px;font-weight:bold'>"+sHtml+"</span>");
				remainTd.html("<span style='margin-left: 25px;font-weight:bold'>"+rHtml+"</span>");
			}
}

function isAboveLimit(date, scheduled){
	var limit = NightLength[date]*60;
	if(scheduled>limit){
		return 1;
	}else{
		return 0;
	}
}


/* Revise table layout functions
 * By Nikhil, 10/09/2013
 * */
function generateTable(supernova, planlist, header) {
	var rowsize = 0;
	var colsize = 0;
	var result = new Array();

	//Row index and row total
	var j = 0;
	var totalFreq = new Array();

	//column index and column total
	var i = 0;
	var totalTime = new Array();

	for ( var star in planlist) {
		totalFreq.push(0);
		setFreq = supernova[star].obsTimes; //Come from the user specification: observation times
		var item = [ star, supernova[star].z, supernova[star].bpeak.substring(5)];
		for (i = 0; i < planlist[star].length; i++) {
			value = "";
			if (planlist[star][i][1] == 1) {
				totalFreq[j] = totalFreq[j] + 1;
				if (!totalTime[i]) {
					totalTime[i] = planlist[star][i][2];
				} else {
					totalTime[i] = totalTime[i]
							+ planlist[star][i][2];
				}
				value = "<input type='checkbox' onclick='recalculate(this)' checked='checked' name='"
						+ planlist[star][i][0]
						+ "' value='"
						+ planlist[star][i][2]
						+ "'/> "
						+ "<span style='color:blue'>"+planlist[star][i][2] + "</span>";
			} else {
				if (!totalTime[i]) {
					totalTime[i] = 0;
				}
				value = "<input type='checkbox' onclick='recalculate(this)' name='"
						+ planlist[star][i][0]
						+ "' value='"
						+ planlist[star][i][2]
						+ "'/> "
						+ "<span style='color:#D0D0D0'>"+planlist[star][i][2] + "</span>";
			}

			item.push(value);
		}
		if(totalFreq[j]<setFreq){//If the observation times is less than the specified times
			item[0] = "<span style='color:red'>"+star+"</span>";
			item.push("<span style='color:red;font-weight:bold'>"+totalFreq[j]+"/"+setFreq
			+"</span>"+"<input type='hidden' value='"+setFreq+"'>");
		}else{
			item[0] = "<span style=''>"+star+"</span>";
			item.push("<span style='font-weight:bold'>"+totalFreq[j]+"/"+setFreq
			+"</span><input type='hidden' value='"+setFreq+"'>");
		}
		result.push(item);
		j++;
	}

		rowsize = j;
		colsize = i + 2;

		var foot1 = [ "<span style='font-weight:bold'>Total Allocated</span>","","" ];
		var foot2 = [ "<span style='font-weight:bold'>Total Scheduled</span>","","" ];
		var foot3 = [ "<span style='font-weight:bold'>Total Remaining</span>","","" ];
		for (k = 0; k < i ; k++) {
			foot1.push("<span style='margin-left: 25px;font-weight:bold'>"
			+NightLength[dateList[k]]*60+"</span>");
			var id="dateHeader"+k;
			if(remain<0){
				header.push({"sTitle":"<span id='"+id+"' style='color:red'>"+dateList[k].substring(5)+"</span>"});
				foot2.push("<span style='margin-left: 25px;font-weight:bold;color:red'>"
					+totalTime[k]+"</span>");
				var remain = NightLength[dateList[k]]*60-totalTime[k];
					foot3.push("<span style='margin-left: 25px;font-weight:bold;color:red'>"
					+remain+"</span>");
			}else{
				header.push({"sTitle":"<span id='"+id+"'  style=''>"+dateList[k].substring(5)+"</span>"});
				foot2.push("<span style='margin-left: 25px;font-weight:bold'>"
					+totalTime[k]+"</span>");
				var remain = NightLength[dateList[k]]*60-totalTime[k];
					foot3.push("<span style='margin-left: 25px;font-weight:bold'>"
					+remain+"</span>");
			}
		}

		header.push({"sTitle" : "#Times"});
		foot1.push("");
		foot2.push("");
		foot3.push("");

		result = result.sort(Comparator);  //Sort the supernova according to RedShift(z)
		result.push(foot1,foot2,foot3);

		return {
			"plan" : result,
			"header" : header,
			"rowsize" : j,
			"columnsize" : i
		};
}

/******************************Revision by Nikhil End ****************/

(function($, window, document, undefined) {

    // Create a GLOBAL namespace to hold global variables
        // Use the Module pattern to maintain the private and public variables separately

        var GLOBAL = (function(){
            var my_global = {};

            if(isUserLoggedIn())
                my_global.userID = getUserId();     // store the logged in user's ID
            else
                my_global.userID = -1;  // set an unreachable value so that we can handle it silently
            /*
            * Store the user's current lists and associated objects in this array.
            * Notes: 1) Reduce the number of AJAX calls made to obtain lists
            * 2) It isn't expensive to search through an array by key (key is the list name)
            * 3) Format: master_objects_array[list_name][SN_name] = {}, where the object contains
            *    properties that are specific to the object (e.g. type, mag, R.A., Dec., etc.)
            */
            my_global.master_lists_array = [];
            my_global.master_objects_array = [];

            my_global.objects_list = [];

            //my_global.experimentSetUp = false;

            /*
            * Store all of the user's experiment parameters in this array.
            * Notes: 1) Reduce the number of AJAX calls made to obtain lists
            * 2) It isn't expensive to search through an array by key (key is the list name)
            * 3) Format: master_experiments_array[list_name][SN_name] = {}, where the object contains
            *    properties that are specific to the object (e.g. type, mag, R.A., Dec., etc.)
            */
            my_global.master_experiments_array = [];

            return my_global;
        }());

        // INIT Module - handles the initialization of user specific items - e.g. lists/objects, experiment parameters, plans
        // Revealing module pattern
        var INIT = (function(){
            var ret_val = {};

            // public methods
            var initExperiments = function(){
                GLOBAL.master_experiments_array = [];
                var tester = true;

                sendAjax(
                    "POST",
                    baseURLs.EXPERIMENT_URL,
                    { user_id: GLOBAL.userID, retrieveExperiments: 1 },
                    "json",
                    function(json_msg){
                        var id_ar = [], exp_name_ar = [];   //temporary arrays to store the experiment ids and names
                        $.each(json_msg, function(index, elem){
                            id_ar.push(elem[0]);
                            exp_name_ar.push(elem[1]);
                        });
                        GLOBAL.master_experiments_array['ids'] = id_ar;
                        GLOBAL.master_experiments_array['names'] = exp_name_ar;

                        if(GLOBAL.userID == 101) {
                            populateDropdown("myExperimentSelector", GLOBAL.master_experiments_array['names'],
                                { 'selected': 'A_TEST_CASE', 'onchange': experimentChangeHandler }
                            );
                            if(tester)
                                $('#myExperimentSelector').trigger('change');
                        } else {
                            populateDropdown("myExperimentSelector", GLOBAL.master_experiments_array['names'],
                                { 'selected': 'none', 'onchange': experimentChangeHandler }
                            );
                        }
                    },
                    function(jqXHR, textStatus, errorThrown){
                        console.log("Error: " + errorThrown + "   " + textStatus);
                    }
                );
            };

            var initLists = function(){
                GLOBAL.master_lists_array = [];
                sendAjax(
                    "POST",
                    baseURLs.LIST_URL,
                    { retrieveLists: 1, _userID: GLOBAL.userID },
                    "json",
                    function(json_msg){
                        var id_ar = [], list_name_ar = [];   //temporary arrays to store the experiment ids and names
                        $.each(json_msg, function(index, elem){
                            id_ar.push(elem[0]);
                            list_name_ar.push(elem[1]);
                        });
                        GLOBAL.master_lists_array['ids'] = id_ar;
                        GLOBAL.master_lists_array['names'] = list_name_ar;

                        console.log(GLOBAL.master_lists_array['names']);

                        if(GLOBAL.userID == 101) {
                            populateDropdown("scheduleListSelector", GLOBAL.master_lists_array['names'],
                                { 'selected': 'A_TEST_CASE', 'onchange': listChangeHandler }
                            );
                        } else {
                            populateDropdown("scheduleListSelector", GLOBAL.master_lists_array['names'],
                                { 'selected': 0, 'onchange': listChangeHandler }
                            );
                        }
                        $( '#scheduleListSelector' ).trigger('change');
                        $( '#listManagementListSelector' ).trigger('change');
                    },
                    function(jqXHR, textStatus, errorThrown){
                        console.log(textStatus + ": " + errorThrown);
                    }
                );
                // $.ajax({
                //         type: "POST",
                //         dataType: "json",
                //         url: baseURLs.LIST_URL,
                //         data: { retrieveLists: 1, _userID: GLOBAL.userID }
                // }).done().fail();
            };

            // add the methods to the return object
            ret_val.initExperiments = initExperiments;
            ret_val.initLists = initLists;

            return ret_val;
        }());

        /////////////////////////////////////
        //  Save Module
        //
        ////////////////////////////////////
        var SAVE = (function(){
            var ret_val = {};

            // Public methods
            var saveExperiment = function(planParams){
                $.ajax({
                    type: "POST",
                    url: baseURLs.EXPERIMENT_URL,
                    data: {json_str: JSON.stringify(planParams), saveExpParams: 1},
                    dataType: "json"
                }).done(function(json_msg){
                    console.log(Object.keys(json_msg));

                    // Update the experiment list
                    UPDATE.updateExperiments({'selected': $('#experimentName').val()});
                    // After updating the configurations dropdown, trigger the dropdown to update the objects below
                    // NOTE: the object will only be associated with the configuration it was saved to, the values will
                    // not be copied to new configurations created using old configurations' parameters

                    $( '#saveNewExperimentDialog' ).dialog( 'close' );

                    create("basic-template", { title: 'Success!', text: 'Experiment parameters saved!' }, { custom: true } );
                    //GLOBAL.experimentSetUp = false;
                    $( '#updateExperiment' ).button('option', 'disabled', false);

                    // This is used to make sure the objects below are updated correctly - we have to set a timeout because the UPDATE
                    // module takes a little bit of time to recreate the dropdown
                    setTimeout(function(){
                        console.log("BLAHSKI: " + $( "#myExperimentSelector option" ).filter( ":selected" ).val());
                        $( '#myExperimentSelector' ).trigger('change');
                    }, 1000);
                }).fail(function(jqXHR, textStatus, errorThrown){
                    console.log(textStatus + ": " + errorThrown);
                    create("error-template", { title: 'Error!', text: 'There was an error in saving your parameters.' }, { custom: true } );
                    var new_error = new SNError( 4, "There was an error in saving your parameters. Please make sure you entered valid values for each field.", null, $( '#feedbackDisplay'), 0);
                    new_error.displayError();
                });
            };

            var saveList = function(options){
                /* Use JSON.stringify to serialize the array in order to pass it in the POST AJAX call
                * TODO: Take a look at $.params for serializing
                */
                var selected, listInfo, objectIDList;
                var widget = $('#supernovae_dialog').data("supernovae");

                if (options.hasOwnProperty('selected'))
                    selected = options['selected'];
                else
                    selected = 0;   // technically shouldn't come here...

                if (options.hasOwnProperty('objectList'))
                    objectIDList = options['objectList'];

                if (options.hasOwnProperty('listInfo'))
                    listInfo = options['listInfo'];     // stores the current list's information (name and description)

                $.ajax({
                    type: "POST",
                    url: baseURLs.LIST_URL,
                    data: { save: 1, '_listInfo': JSON.stringify(listInfo), '_userID': GLOBAL.userID, '_objectIDs': JSON.stringify(objectIDList) },
                    dataType: "json"
                }).done(function(json_msg){
                    if(json_msg['success']){
                        $( '#updateList' ).button("option", "disabled", false); //enable the update button so user can update the newly created list
                        create("basic-template", { title: 'Success!', text: 'List successfully saved.' }, { custom: true } );
                        //that._cur_user_list = [listName, listDesc];
                        widget._cur_user_list = listInfo;
                        widget._setUpSearchResults(selected);
                        UPDATE.updateLists();
                    }
                }).fail(function(jqXHR, textStatus, errorThrown) {
                    console.log(textStatus, errorThrown); // use alert() if you prefer
                });
            };

            ret_val.saveExperiment = saveExperiment;
            ret_val.saveList = saveList;

            return ret_val;
        })();

        /////////////////////////////////////
        //  Update Module
        //
        ////////////////////////////////////
        var UPDATE = (function(){
            var ret_val = {};
            // cache the SNeT instance
            var widget = $('#supernovae_dialog').data("supernovae");

            // public methods
            var updateExperiments = function(options){
                var selected;

                if(options.hasOwnProperty('selected'))
                    selected = options['selected'];
                else
                    selected = 'none';

                console.log("SELECTED: " + selected);

                sendAjax(
                    "POST",
                    baseURLs.EXPERIMENT_URL,
                    { user_id: GLOBAL.userID, retrieveExperiments: 1 },
                    "json",
                    function(json_msg){
                        var id_ar = [], exp_name_ar = [];
                        $.each(json_msg, function(index, elem){
                            id_ar.push(elem[0]);
                            exp_name_ar.push(elem[1]);
                        });
                        GLOBAL.master_experiments_array['ids'] = id_ar;
                        GLOBAL.master_experiments_array['names'] = exp_name_ar;

                        populateDropdown("myExperimentSelector", GLOBAL.master_experiments_array['names'],
                            { 'selected': selected, 'onchange': experimentChangeHandler }
                        );
                    },
                    function(jqXHR, textStatus, errorThrown){
                        console.log("Error: " + errorThrown + "   " + textStatus);
                    }
                );
            };

            var updateLists = function(options){
                var selected;

                if (options) {
                    if(options.hasOwnProperty('selected'))
                        selected = options['selected'];
                    else
                        selected = 0;   // the first index of the lists
                }

                sendAjax(
                    "POST",
                    baseURLs.LIST_URL,
                    { retrieveLists: 1, _userID: GLOBAL.userID },
                    "json",
                    function(json_msg){
                        GLOBAL.master_lists_array = []; // reset the list before pushing the lists on again (including new ones)
                        var id_ar = [], list_name_ar = [];   //temporary arrays to store the experiment ids and names
                        $.each(json_msg, function(index, elem){
                            id_ar.push(elem[0]);
                            list_name_ar.push(elem[1]);
                        });
                        GLOBAL.master_lists_array['ids'] = id_ar;
                        GLOBAL.master_lists_array['names'] = list_name_ar;
                        console.log(GLOBAL.master_lists_array['names']);

                        if(GLOBAL.userID == 101) {
                            populateDropdown("scheduleListSelector", GLOBAL.master_lists_array['names'],
                                { 'selected': 'A_TEST_CASE', 'onchange': listChangeHandler }
                            );
                        } else {
                            populateDropdown("scheduleListSelector", GLOBAL.master_lists_array['names'],
                                { 'selected': selected, 'onchange': listChangeHandler }
                            );
                        }
                        $( '#scheduleListSelector' ).trigger('change');
                        $( '#listManagementListSelector' ).trigger('change');
                    },
                    function(jqXHR, textStatus, errorThrown){
                        console.log(textStatus + ": " + errorThrown);
                    }
                );
            }

            ret_val.updateExperiments = updateExperiments;
            ret_val.updateLists = updateLists;

            return ret_val;
        }());

    /////////////////////////////////////
    //  Reset Module
    //
    ////////////////////////////////////
    var RESET = (function(){
        var ret_val = {};

        // public methods
        // This method resets the full display for each tab
        var resetDisplays = function(options){
            var $display = $( '.displaySupernovaContent' );
            $display.find('input').val('');
        };

        ret_val.resetDisplays = resetDisplays;
        return ret_val;
    })();

    /////////////////////////////////////////
    //  Change handlers for INIT and UPDATE
    //  modules
    ////////////////////////////////////////
    var experimentChangeHandler = function() {
        var widget = $('#supernovae_dialog').data("supernovae");
        tester = false;
        //GLOBAL.experimentSetUp = false;     // set the experiment setup flag to false each time a new configuration is selected

        GLOBAL.objects_list = [];
        // Retrieve the selected configuration name
        var selectedExpName = $( "#myExperimentSelector option" ).filter( ":selected" ).val();

        console.log(selectedExpName);
        // retrieve the index of the selected experiment name
        var idx = $.inArray(selectedExpName, GLOBAL.master_experiments_array['names']);
        // retrieve the associated experiment ID
        if (idx != -1) {
            var exp_id = parseInt(GLOBAL.master_experiments_array['ids'][idx]);

            $.ajax({
                type: "POST",
                url: baseURLs.EXPERIMENT_URL,
                data: {'user_id': GLOBAL.userID, 'exp_id': exp_id, retrieveExpParams: 1},
                dataType: "json"
            }).done(function(json_msg){
                if (json_msg['objects']) {
                    GLOBAL.objects_list = json_msg['objects'];
                    var idx_ar = [];
                    // Loop through the objects and add the IDs to a temporary array
                    $.each(json_msg['objects'], function(index, elem){
                        idx_ar.push(elem['unique_id']);
                    });
                    console.log(idx_ar);
                    // Loop through the IDs and check to make sure the ID is unique, otherwise remove it from the objects list
                    $.each(idx_ar, function(index, elem){
                        var index;
                        while((index = $.inArray(elem, idx_ar)) > -1 && elemCount(idx_ar, elem) > 1)
                        {
                            //console.log("ELEM: " + elem + " INDEX: " + index + " COUNT: " + elemCount(idx_ar, elem));
                            GLOBAL.objects_list.splice(index, 1);
                            idx_ar.splice(index, 1);
                        }
                    });
                    console.log(GLOBAL.objects_list);
                    console.log("Calling initExperiments setupexperiment...");
                    widget.setUpExperiment(GLOBAL.objects_list, 'objects');
                }
                console.log("Calling initExperiments setupexperiment with no objects...");
                if (!json_msg['objects'])
                    objectsInvolved = false;

                    widget.setUpExperiment(json_msg);
                // if ($( '#scheduleAccordion' ).accordion('option', 'active') != 0)
                //     $( '#scheduleAccordion' ).accordion('option', 'active', 0); //manually set this if it is not already open
            }).fail(function(jqXHR, textStatus, errorThrown){
                console.log(textStatus + ": " + errorThrown);
            });
        } else if (selectedExpName === "none") {
            //GLOBAL.experimentSetUp = false;
            widget.setUpExperiment({'num_nights': ''});
        }
    };

    var listChangeHandler = function() {
        var selectedList = $( "#scheduleListSelector option" ).filter(":selected").val();   //obtain the selected option
        var selectedListID = GLOBAL.master_lists_array['ids'][$.inArray(selectedList, GLOBAL.master_lists_array['names'])];
        console.log(selectedListID);

        /* Make an AJAX POST request to retrieve all the objects in the selected list */
        $.ajax({
            type: "POST",
            dataType: "json",
            url: baseURLs.LIST_URL,
            data: { objects: 1, _selectListID: selectedListID, _userID: GLOBAL.userID }
        }).done(function(json_msg){
            // cache the json returned in a private variable in this closure
            var res = json_msg;

                GLOBAL.master_objects_array = [];     //reset the array each time a new list is selected - the master array only contains the objects of the currently viewed list
                $.each(res['0'], function(){
                    var object_info = {};
                    object_info.id = this.object_id;
                    object_info.name = this.object_name.split(",").splice(1);
                    object_info.ra = this.object_ra.toFixed(5);
                    object_info.dec = this.object_dec.toFixed(5);
                    object_info.type = this.object_type;
                    object_info.mag = (this.object_mag == null) ? 0 : this.object_mag;
                    object_info.redshift = (this.object_redshift == null) ? 0 : this.object_redshift;
                    object_info.phase = this.object_phase;
                    object_info.bpeak = '';
                    object_info.obsgap = 1;
                    object_info.obstimes = '';
                    object_info.priority = 0.5;
                    object_info.message = res['messages'][0][this.object_id];

                    GLOBAL.master_objects_array[this.object_name.split(",")[0]] = object_info;
                });
                var widget = $('#supernovae_dialog').data("supernovae");
                widget.populateObjects();
                console.log(GLOBAL.objects_list);
                console.log("Calling initLists setupexperiment...");
                widget.setUpExperiment(GLOBAL.objects_list, 'objects');
                scheduleTable.fnAdjustColumnSizing();
        });
    };

    $.widget("ui.supernovae", $.ui.dialog, {
        options: {
            // default options
            //autoResize: true,
            width: 1500,
            height: 900,
            minHeight: 300,
            maxWidth: 1500,
            autoOpen: false,
            draggable: false,
            title: "SNeT v0.2",
            resizable: false,
            modal: true,
            fluid: true
        },


        /* Function to animate the transition from scheduler to view plan - "auto Switch" */
        _autoSwitch: function(percentage, index) {
                    this.progressBar.children( ".ui-progressbar-value" ).animate({width: percentage}, 600, 'swing'); //animate the progressbar

                    /* Get the childList, remove the selectedSupernovaTab class from all, find the second child, add the class back
                    * then hide the tabContent and then fade in the correct div matching the second child
                     */
                    var searchResults = this.childList.removeClass("selectedSupernovaTab").eq(index).addClass("selectedSupernovaTab");

                    this.tabContent.hide(); //Hide all tab content
                    $(searchResults.find("a").attr("href")).fadeIn(600);   //find the div that the link is referring to and fade it in
        },

        /* Function that sanitizes the search field before submitting the search query. */
        _clean: function(type) {
                console.log("Cleaning...");
                $("div.item").remove();
                if (type == 0) {
                    $("#obj_name").val('');

                    $("#obj_ra").val('');
                    $("#obj_dec").val('');
                    $("#obj_epsilon").val('');
                } else if(type == 1) {
                    $('#browse_all').prop('checked', false);

                    $("#obj_ra").val('');
                    $("#obj_dec").val('');
                    $("#obj_epsilon").val('');
                } else if(type == 2) {
                    $('#browse_all').prop('checked', false);

                    $("#obj_name").val('');
                } else if(type == 3) {
                    $('#browse_all').prop('checked', false);
                    $("#obj_name").val('');
                    $("#obj_ra").val('');
                    $("#obj_dec").val('');
                    $("#obj_epsilon").val('');
                } else {
                    ;
                }
        },

        /*
            Set up all the click handlers, and initialize variables for all sections.
            Do it here because that way everything will be initialized before loading
            SNeT.
        */
        _create: function() {
                    $.ui.dialog.prototype._create.apply(this);  //call the constructor of the ui.dialog widget

                    //set the variables needed - cache when appropriate
                    var self = this.element,
                        that = this,
                        options = self.options;

                    this.childList = self.find( "#supernovaTabs > li" );
                    this.progressBar = self.children( "#progressbarSupernova" );
                    this.tabContent = $( ".displaySupernovaContent" );

                    //Create a global array that will contain the objects which have fields not entered
                    that.errorObjs = [];

                    var searchTabs = $( "#searchTabs" ),
                        searchResetButtons = this.tabContent.find("#searchButtons").children(),
                        scheduleButtons = $( "#scheduleButtons" ).children(),
                        createNewListDialog = $( "#createNewListDialog" ),
                        generatePlanDialog = $( "#generatePlanDialog" );

                    var pvalue = 0;
                    that.selectedSearchIndex = 0;

                    this.progressBar.progressbar({value: 19.8});    // set up the progress bar

                    this.childList.on("click", function(event){
                       event.preventDefault();
                       that.childList.removeClass("selectedSupernovaTab");
                       $(this).addClass("selectedSupernovaTab");

                        that.tabContent.hide(); //Hide all tab content

                        //handle the progressbar animations to align correctly with the "tabs" above
                        switch($(this).index()){
                            case 0:
                                pvalue = "19.8%";
                                break;
                            case 1:
                                if(!that._search_results_flag){
                                    $("#searchResultsOverlay").css({"display": "block", "z-index": "10"});
                                    $("#searchResultsSupernova").find("> div:not(:first)").css("visibility", "hidden");
                                } else {

                                }
                                pvalue = "39.6%";
                                break;
                            case 2:
                                if(!that._list_management_flag)
                                    that._setUpListManagement();
                                pvalue = "59.6%";
                                break;
                            case 3:
                                if(!that._scheduler_flag) {
                                    that._setUpScheduler();
                                    // HACK!!!!
                                    // Trigger the dropdown change again to make datatables cooperate...
                                    $( '#scheduleListSelector' ).trigger('change');
                                }
                                pvalue = "79.5%";
                                break;
                            case 4:
                                if(!that._view_plan_flag){
                                    $("#viewPlanOverlay").css("display", "block");
                                    $("#viewPlanSupernova > table, #viewPlanSupernova > div:not(:first)").css("visibility", "hidden");
                                }
                                pvalue = "100%";
                                break;
                        }
                       that.progressBar.children( ".ui-progressbar-value" ).animate({width: pvalue}, 400, 'swing'); //animate the progressbar

                        $($(this).find("a").attr("href")).fadeIn();

                        that._setSearchClickHandlers(that.selectedSearchIndex);
                    });

                    $( ".snbuttons" ).button();

                    /* Make sure the numeric input fields only accept numbers! */
                    $( ".numericInput" ).keydown(function(event){
                        //Allow: backspace, delete, tab, escape, and enter, commas, and decimal points
                        if ( event.keyCode == 46 || event.keyCode == 8 || event.keyCode == 9 || event.keyCode == 27 || event.keyCode == 13 || event.keyCode == 110 || event.keyCode == 190 || event.keyCode == 188 ||
                            // Allow: Ctrl + A
                            (event.keyCode == 65 && event.ctrlKey === true) ||
                            // Allow: home, end, left, right
                            (event.keyCode >= 35 && event.keyCode <= 39) ) {
                            return;
                        } else {
                            if (event.shiftKey || (event.keyCode < 48 || event.keyCode > 57) && (event.keyCode < 96 || event.keyCode > 105 )) {
                                event.preventDefault();
                            }
                        }

                    });

                    // Call the RESET module to reset everything before starting
                    RESET.resetDisplays();

                    /******************************/
                    /*   Search Initializations   */
                    /******************************/

                        that.tempNewListCopy = null;
                        advOptions = {}, advOptions['param'] = [], advOptions['operator'] = [], advOptions['value'] = [];
                        searchTabs.tabs({
                            create: function(event, ui) {
                                console.log(that.selectedSearchIndex);
                                that._setSearchClickHandlers(that.selectedSearchIndex);
                            },
                            select: function(event, ui) {
                                that.selectedSearchIndex = ui.index;
                                console.log(that.selectedSearchIndex);
                                that._setSearchClickHandlers(that.selectedSearchIndex);
                            }
                        }).addClass('ui-tabs-vertical ui-helper-clearfix');

                        /* Set the event handlers for the advanced search functionality. */
                        $supernova_adv_search = $( "#searchSupernovaAdvanced" );

                        // $("select[id*='supernovaAdvParams']").each(function(index){
                        //     //console.log($(this).attr('id'));
                        //     var cur_id = '#' + $(this).attr('id');
                        //     $(cur_id).on('change', function(){
                        //         advOptions['param'][index] = parseInt($( cur_id + " option" ).filter( ":selected" ).val());
                        //         //console.log(advOptions['param'][index]);
                        //     }).trigger('change');
                        // });

                        $( '#supernovaAdvParams' ).on('change', function(){
                            advOptions['param'][$(this).parent().index()-1] = parseInt($( "#supernovaAdvParams option" ).filter( ":selected" ).val());
                            console.log($( "#supernovaAdvParams option" ).filter( ":selected" ).val());
                        }).trigger('change');

                        $( '#supernovaAdvOperators' ).on('change', function(){
                            advOptions['operator'][$(this).parent().index()-1] = parseInt($( "#supernovaAdvOperators option" ).filter( ":selected" ).val());
                        }).trigger('change');

                        $( '#supernovaAdvValue' ).on('change', function(){
                            advOptions['value'][$(this).parent().index()-1] = $("#supernovaAdvValue").val();
                        });

                        $( "#searchSupernovaAdvanced" ).on('click', '.addAdvValue', function(event){
                            //var html_str = $( ".supernovaAdvOption:last").html();
                            //$( "#searchSupernovaAdvanced" ).find( ".supernovaAdvOption:last" ).after('<div class="supernovaAdvOption"></div>');
                            //$( ".supernovaAdvOption:last" ).html(html_str);
                            if($.trim($('#supernovaAdvValue').val()) !== "" && $.trim($('#supernovaAdvValue').val()).length > 0){
                                that._clearTFCSS($("#supernovaAdvValue"));
                                $(this).parent().next("label").text('');
                                $( this ).siblings( '#supernovaAdvParams, #supernovaAdvOperators, #supernovaAdvValue' ).prop('disabled', true);
                                $( this ).button("disable");
                                console.log(advOptions);
                                var test = $( this ).parent().nextAll('.supernovaAdvOption').first();
                                if(test){
                                    test.css("visibility", "visible");
                                    //$supAdvOption.appendTo( '#searchSupernovaAdvanced' );
                                }
                            } else {
                                var new_error = new SNError( 2, "Please enter a value.", $('#supernovaAdvValue'), $(this).parent().next("label"), 0);
                                new_error.displayError();
                            }
                        });


                        $( "#searchSupernovaAdvanced" ).on('click', '.deleteAdvValue', function(event){
                            //$( this ).prev().button("enable");
                            //if($(this).parent().next('.supernovaAdvOption').length > 0 &&  )
                            var test = $( this ).parent().nextAll('.supernovaAdvOption').first().css('visibility') === "visible";
                            if(test)
                                $supAdvOption = $( this ).parent(".supernovaAdvOption").detach();
                        });

                        /* Set change handlers for the Ordering dropdowns */
                        // according to jQuery 1.7 docs, using .filter(":selected") increases performance rather than use the :selected selector
                        $( "#supernovaOrderBy" ).change( function() {
                            that._supernovaOrderBy = parseInt($("#supernovaOrderBy option").filter(":selected").val());
                        }).trigger( "change" );

                        $( "#supernovaSortOrder" ).change( function() {
                            that._supernovaSortOrder = $("#supernovaSortOrder option").filter(":selected").val();
                        }).trigger( "change" );

                        $( "#resetsearchsupernova" ).on('click', function(){
                            $search_panel = $(this).parents( "#searchSupernova" );
                            $search_panel_textfields = $search_panel.find( 'input[type="text"]' );
                            $search_panel_textfields.val("");
                            $search_panel.find( 'input[type="checkbox"' ).prop("checked", false);
                            that._clearTFCSS($search_panel_textfields);
                            $search_panel.find( '.snerror' ).text("");
                        });


                    /**************************************/
                    /*   Search Results Initializations   */
                    /**************************************/
                        /* Global array that holds the dragged elements - this should be initialized in create so it stays for the duration of the usage of the extension */
                        aDragged = [];  //store the elements that were dragged to the new list - maintain CSS styling
                        listNamesArr = [];  // store the list names for the current logged in user
                        con_drag_and_user_lists = [];   //combination of the user's current list objects and newly dragged objects - e.g. row_115

                        $container = $( "#notycontainer" ).notify({
                            expires: 4000,
                            speed: 500
                        }).css("z-index", "3000");

                        /*---delete row from DataTable---*/
                        $("a[class=remove_new_list]").livequery('click', function(e){
                            e.preventDefault();
                            $the_tr = $(this).parents('tr');
                            var curr_table = $the_tr.parents('table').dataTable();
                            var the_pos = curr_table.fnGetPosition(this.parentNode.parentNode);
                            curr_table.fnDeleteRow(the_pos);

                            // Reset the appropriate rows
                            $("#" + con_drag_and_user_lists[the_pos]).draggable("enable").on('click', function(event){ $(this).toggleClass('row_selected'); }).removeClass("row_dragged");   //re-enable the drag-n-drop functionality for the deleted rows
                            con_drag_and_user_lists.splice(the_pos, 1);
                            aDragged.splice(the_pos, 1);
                            if(that.userListIDs)
                                that.userListIDs.splice(the_pos, 1);    //if the user removes an item from a selected list, remove it from the global list

                        });

                        /*---get more info from row in DataTable---*/
                        $("a[class=get_row_info]").livequery('click', function(e){
                            e.preventDefault();
                        });

                        $( "#copySearchResults" ).on('click', function(event){
                            var rows = oTable.fnGetNodes();
                            var row_length = $(rows[0]).find('td').length;
                            var copy_draggableIDs = [];
                            var cells = [], addedData = [];
                            for(var i=0, oLen = rows.length; i < oLen; ++i){
                                cells = [];
                                /* Take care of not copying over duplicate objects - check if the row has already been dragged over to the other list. */
                                if($.inArray($(rows[i]).attr('id'), aDragged) < 0 && $.inArray($(rows[i]).attr('id'), con_drag_and_user_lists) < 0) {
                                    cells.push('<a class="remove_new_list" href="#"><span class="ui-icon ui-icon-circle-close"></span></a>');
                                    for(var j=0; j < row_length; ++j){
                                        cells.push($(rows[i]).find('td').eq(j).text());
                                    }
                                    addedData.push(cells);
                                    copy_draggableIDs.push($(rows[i]).attr("id"));
                                } else {

                                }
                            }
                            newListTable.fnAddData(addedData);

                            /* Take care of disabling the objects from the search results. */
                            for (var i = 0, len = copy_draggableIDs.length; i < len; ++i) {
                                $("#" + copy_draggableIDs[i]).off('click').removeClass("row_selected").addClass("row_dragged").draggable("disable");

                                /* Preferred method */
                                aSelected.splice( $.inArray(copy_draggableIDs[i], aSelected), 1);

                                var draggedIndex = $.inArray(copy_draggableIDs[i], aDragged);
                                if ( draggedIndex === -1 ) {
                                    aDragged.push( copy_draggableIDs[i] );
                                    con_drag_and_user_lists.push( copy_draggableIDs[i] );
                                }

                            }

                        });

                        $( "#copySelectionSearchResults" ).on('click', function(event){
                            var rows_selected = $("#searchResultsDisplay tr.row_selected");
                            var num_rows_selected = rows_selected.length;
                            var row_length = $(rows_selected[0]).find('td').length;
                            var copy_draggableIDs = [];
                            var cells = [], addedData = [];

                            for(var i=0; i < num_rows_selected; ++i){
                                cells = [];
                                /* Take care of not copying over duplicate objects - check if the row has already been dragged over to the other list. */
                                if($.inArray($(rows_selected[i]).attr('id'), aDragged) < 0 && $.inArray($(rows_selected[i]).attr('id'), con_drag_and_user_lists) < 0) {
                                    cells.push('<a class="remove_new_list" href="#"><span class="ui-icon ui-icon-circle-close"></span></a>');
                                    for(var j=0; j < row_length; ++j){
                                        cells.push($(rows_selected[i]).find('td').eq(j).text());
                                    }
                                    addedData.push(cells);
                                    copy_draggableIDs.push($(rows_selected[i]).attr("id"));
                                } else {

                                }
                            }
                            newListTable.fnAddData(addedData);

                            /* Take care of disabling the objects from the search results. */
                            for (var i = 0, len = copy_draggableIDs.length; i < len; ++i) {
                                $("#" + copy_draggableIDs[i]).off('click').removeClass("row_selected").addClass("row_dragged").draggable("disable");

                                /* Preferred method */
                                aSelected.splice( $.inArray(copy_draggableIDs[i], aSelected), 1);

                                var draggedIndex = $.inArray(copy_draggableIDs[i], aDragged);
                                if ( draggedIndex === -1 ) {
                                    aDragged.push( copy_draggableIDs[i] );
                                    con_drag_and_user_lists.push( copy_draggableIDs[i] );
                                }
                                // else {
                                //     aDragged.splice( draggedIndex, 1 );
                                // }
                            }

                            //oTable.fnDraw();
                        });

                        $( "#clearList" ).on('click', function(event){
                            newListTable.fnClearTable();    //use built-in DataTables method to clear the entire table
                            console.log(aDragged);
                            for(var i=0, oLen = aDragged.length; i < oLen; ++i){
                                $("#" + aDragged[i]).draggable("enable").on('click', function(event){ $(this).addClass('row_selected'); }).removeClass("row_dragged");   //re-enable the drag-n-drop functionality for the dragged rows
                            }
                            aDragged = [];  //reset the aDragged array
                            con_drag_and_user_lists = [];   //reset the combined array since both dragged and previous objects will be cleared
                        });

                        $( "#saveList" ).button("option", "icons", { primary: "ui-icon-disk" } )
                            .on( 'click', function(event){
                                if(newListTable.fnGetData().length > 0)
                                    createNewListDialog.dialog( 'open' );
                                else
                                    alert("There are no objects in your list!");
                        });

                        $( "#updateList" ).button("option", { "icons": { primary: "ui-icon-refresh" }, "disabled": true } )
                            .on( 'click', function(event){
                                /* Use JSON.stringify to serialize the array in order to pass it in the POST AJAX call
                                * TODO: Take a look at $.params for serializing
                                */

                                // Merges both the dragged items and the items previously in the list - takes into account ones that are removed too
                                var objectIDList = arrayUnique(that.userListIDs.concat(that._retrieveDraggedIDs()));

                                // Update the complete list of dragged and current list objects to be used by datatables
                                con_drag_and_user_lists = arrayUnique(con_drag_and_user_lists.concat(aDragged));

                                $.ajax({
                                    type: "POST",
                                    url: baseURLs.LIST_URL,
                                    data: { save: 1, 'deletion': 1, '_listInfo': JSON.stringify(that._cur_user_list), '_userID': that.userID, '_objectIDs': JSON.stringify(objectIDList) },
                                    dataType: "json",
                                    "error": function(jqXHR, textStatus, errorThrown) {
                                            console.log(textStatus, errorThrown);
                                    }
                                }).done(function(json_msg){
                                    if(json_msg['success']){
                                        create("basic-template", { title: 'Success!', text: 'List successfully updated.' }, { custom: true } );
                                        that._setUpSearchResults(that._cur_user_list[0]);
                                    }
                                });

                                oTable.fnDraw();    //redraw the table to display row enabling/disabling changes
                        });

                        createNewListDialog.dialog({
                            autoOpen: false,
                            width: 400,
                            maxWidth: 400,
                            height: 300,
                            maxHeight: 300,
                            modal: true,
                            resizable: false,
                            zIndex: 1002
                        }).find( "button" ).button()
                            .on('click', function(event){
                                that.tempNewListCopy = null;    //reset the temporary list after the new list has been saved
                                var listName = $( "#newListName" ).val();
                                var listDesc = $( this ).siblings( "textarea" ).val();
                                var objectIDList = null;
                                if(!that._clicked_other_list)
                                    objectIDList = that._retrieveDraggedIDs();
                                else
                                    objectIDList = arrayUnique(that.userListIDs.concat(that._retrieveDraggedIDs()));
                                var listInfo = [listName, listDesc];

                                if(that.validateNewList()) {
                                    SAVE.saveList({ 'selected': listName, 'objectList': objectIDList, 'listInfo': listInfo });
                                    createNewListDialog.dialog("close");
                                }
                        });

                    /*************************************/
                    /*  List Management Initializations  */
                    /*************************************/
                    $( '#renameLMList' ).on('click', function(event){
                        $( '#renameListDialog' ).dialog('open');
                    });

                    $( '#deleteLMList' ).on('click', function(event){
                        var selectedList = $( "#listManagementListSelector option" ).filter(":selected").val();
                        var list_id = GLOBAL.master_lists_array['ids'][$.inArray(selectedList, GLOBAL.master_lists_array['names'])];

                        var answer = confirm( 'Are you sure you want to delete list ' + selectedList + '?' );

                        if (answer) {
                            $.ajax( {
                                type: 'POST',
                                dataType: 'json',
                                url: baseURLs.LISTMANAGEMENT_URL,
                                data: { 'delete': 1, '_list_id': list_id }
                            } ).done( function(json_msg) {
                                listManagementTable.fnClearTable();
                                that._setUpListManagement();
                                // $( "#listManagementListSelector option[value='" + rnListName + "']" ).attr('selected', 'selected');
                                // $( '#listManagementListSelector' ).trigger( 'change' );
                                UPDATE.updateLists();
                                create("basic-template", { title: 'Success!', text: 'Your list was successfully deleted.' }, { custom: true } );
                            } ).fail( function(jqXHR, textStatus, errorThrown) {
                                console.log("Error occurred: " + textStatus + ": " + errorThrown);
                                create("error-template", { title: 'Error!', text: 'There was an error when deleting your list.' }, { custom: true } );
                            } );
                        } else {}

                    });

                    $( '#updateLMList' ).on('click', function(event){
                        console.log(listManagementTable.fnGetData());
                    });

                    $( '#saveNewLMList' ).on('click', function(event){
                        createNewListDialog.dialog( 'open' );
                    });

                    $( '#listPermissions' ).on('click', function(event){
                        $( '#listPermissionsDialog' ).dialog('open');
                    });

                    // Create the rename list dialog
                    $( '#listPermissionsDialog' ).dialog({
                        autoOpen: false,
                        width: 400,
                        maxWidth: 400,
                        height: 300,
                        maxHeight: 300,
                        modal: true,
                        resizable: false,
                        zIndex: 1002
                    });

                    // Create the rename list dialog
                    $( '#renameListDialog' ).dialog({
                        autoOpen: false,
                        width: 400,
                        maxWidth: 400,
                        height: 300,
                        maxHeight: 300,
                        modal: true,
                        resizable: false,
                        zIndex: 1002
                    })//.children( 'label:first' ).children( 'label' ).text($( "#listManagementListSelector option" ).filter(":selected").val())
                    //.end()
                    .find( 'button:last' ).on('click', function(){

                        var selectedList = $( "#listManagementListSelector option" ).filter(":selected").val();
                        var rnListName = $( '#renameListName' ).val();
                        $.ajax( {
                            type: 'POST',
                            dataType: 'json',
                            url: baseURLs.LISTMANAGEMENT_URL,
                            data: { renameList: 1, '_name': rnListName, '_list_id': GLOBAL.master_lists_array['ids'][$.inArray(selectedList, GLOBAL.master_lists_array['names'])] }
                        } ).done( function(json_msg) {
                            listManagementTable.fnClearTable();
                            that._setUpListManagement();
                            $( "#listManagementListSelector option[value='" + rnListName + "']" ).attr('selected', 'selected');
                            $( '#listManagementListSelector' ).trigger( 'change' );
                            UPDATE.updateLists();
                            $( '#renameListDialog' ).dialog('close');
                            create("basic-template", { title: 'Success!', text: 'Your list was successfully renamed to ' + rnListName }, { custom: true } );
                        } ).fail( function(jqXHR, textStatus, errorThrown) {
                            console.log("Error occurred: " + textStatus + ": " + errorThrown);
                            create("error-template", { title: 'Error!', text: 'There was an error in renaming your list.' }, { custom: true } );
                        } );

                    });

                    /*********************************/
                    /*   Scheduler Initializations   */
                    /*********************************/

                    planParams = {};    //global object that holds all the plan parameters
                    clickedObjText = null;
                    $genOptions = $( "#scheduleGeneralOptions" ).find( 'input[type="text"]' );

                    aChecked = false, bChecked = false, updating = false, addingNew = false;
                    var dateArray = [];
                    // initially, disable the experiment saving/updating buttons
                    $( '#saveExperimentParams, #updateExperiment' ).button("option", "disabled", true);

                    // Intialize the configurations and overview accordions
                    $( '#configurationAccordion, #overviewAccordion' ).accordion({
                        autoHeight: false,
                        collapsible: true,
                        active: 0
                    });

                    // set up a blur and keyup handler so when user enters the number of nights, the appropriate number rows can be inserted
                    $( '#scheduleNumNights' ).on('keyup', function(event){
                        //if(!GLOBAL.experimentSetUp) {
                            if($.trim($(this).val()) !== "" && $(this).val !== "0") {
                                var html_str = "<p>Enter the number of nights, hours, and indicate whether it is a half night.</p>";
                                $( '#saveExperimentParams' ).button("option", "disabled", false);
                                // If the user has already selected an experiment, allow them to update the current one by changing the number of nights
                                // and dates
                                if($('#myExperimentSelector').val() !== 'none')
                                    $( '#updateExperiment' ).button("option", "disabled", false);
                                for (var k = 0; k < parseInt($(this).val()); k++) {
                                    html_str += '<p><label>Night ' + (k+1) + ': <input type="text" id="schNight' + k + '" size="15" placeholder="Choose a date."/></label>&nbsp;&nbsp;';
                                    html_str += '<label>Hours: <input type="text" id="schNHours' + k + '" size="15" placeholder="Number of hours."/></label>&nbsp;';
                                    html_str += '<input type="checkbox" value="" id="halfA' + k + '"/><label>a</label>&nbsp;&nbsp;';
                                    html_str += '<input type="checkbox" value="" id="halfB' + k + '"/><label>b</label></p>';
                                }
                                $( '#scheduleDatesAndHours' ).empty().html(html_str);   // set the html of the div to the string created above
                                makeNightPicker();  // for dynamic elements, use a function to set the datepicker instances *****
                                setHalfNightHandlers();
                            } else {
                                $( '#saveExperimentParams, #updateExperiment' ).button("option", "disabled", true);
                                $( '#scheduleDatesAndHours' ).empty();
                            }
                            if(event.keyCode == 13) // if the user hits return, manually call blur
                                $(this).keyup();
                        //}
                    });

                    var makeNightPicker = function () {
                        $('[id^="schNight"]').datepicker({
                            dateFormat: 'yy-mm-dd',
                            beforeShowDay: function(date){
                                dateArray = [];
                                $.each($('[id^="schNight"]'), function(index){
                                    dateArray.push($('#schNight' + index).val());
                                });
                                var string = jQuery.datepicker.formatDate('yy-mm-dd', date);
                                return [ dateArray.indexOf(string) == -1 ];
                            },
                            onSelect: function(dateText, inst) {
                                dateArray.push(dateText);
                            }
                            // onSelect: function(dateText, inst){
                            //     var $curField = $(this);
                            //     var $curFieldText = $curField.val();
                            //     var $curFieldID = $curField.attr('id');
                            //     $.each($('[id^="schNight"]'), function(index){
                            //         var id_str = '#schNight' + index;
                            //         if($curFieldID !== id_str.substring(1) && $( id_str ).val() === dateText){
                            //             alert("You have already chosen that night!");
                            //             if($.trim($curField.val() !== ''))
                            //                 $curField.val($curFieldText);
                            //             else
                            //                 $curField.val('');
                            //             return false;
                            //         }
                            //     });
                            // }
                        });
                    };

                    var setHalfNightHandlers = function () {
                        $('[id^="halfA"]').on('change', function(event){
                            var index = $(this)[0].getAttribute("id");
                            index = index[index.length - 1];
                            if($('#halfB' + index).is(':checked')) {
                                $('#halfB' + index).prop('checked', false).val("");
                                $(this).prop('checked', true);
                            }
                            console.log(aChecked);
                            if(aChecked) {  // if a is checked, uncheck it and unset the value
                                $(this).prop('checked', false);
                                $(this).attr('value', '');
                            } else if(!aChecked){   // if a is not checked, check it and set the value
                                $(this).prop('checked', true);
                                $(this).attr('value', 'a');
                            }

                            console.log("VAL: " + $(this).val());
                            aChecked = !aChecked;
                        });

                        $('[id^="halfB"]').on('change', function(event){
                            var index = $(this)[0].getAttribute("id");
                            index = index[index.length - 1];
                            if($('#halfA' + index).is(':checked')) {
                                $('#halfA' + index).prop('checked', false).val("");
                                $(this).prop('checked', true);
                            }
                            if(bChecked) {
                                $(this).prop('checked', false);
                                $(this).attr('value', '');
                            } else if(!bChecked){
                                $(this).prop('checked', true);
                                $(this).attr('value', 'b');
                            }
                            console.log("VAL: " + $(this).val());
                            bChecked = !bChecked;
                        });
                    };

                    $( "#saveExperimentParams" ).on('click', function(event){
                            $( '#saveNewExperimentDialog' ).dialog( 'open' );
                            $( '#experimentName' ).val('');
                    });

                    // Set up the Generate Plan dialog, set options, and the event handlers
                    $( '#saveNewExperimentDialog' ).dialog({
                        autoOpen: false,
                        width: 350,
                        maxWidth: 350,
                        height: 200,
                        maxHeight: 200,
                        modal: true,
                        resizable: false,
                        zIndex: 1002
                    });

                    // Handle saving a new experiment parameters.
                    $( "#saveExperiment" ).on('click', function(event){
                        // Validate the experiment parameters first
                        if(that.validateExperiment()) {
                            that._createExperimentParams();
                            planParams['exp_name'] = $.trim($('#experimentName').val());
                            console.log(JSON.stringify(planParams));
                            SAVE.saveExperiment(planParams);
                        } else {
                            create("error-template", { title: 'Error!', text: 'There was an error in saving your parameters.' }, { custom: true } );
                            var new_error = new SNError( 4, "There was an error in saving your parameters. Please make sure you entered valid values for each field.", null, $( '#feedbackDisplay'), 0);
                            new_error.displayError();
                        }
                    });

                    $( '#updateExperiment' ).on('click', function(event){
                        var selectedExpName = $( "#myExperimentSelector option" ).filter( ":selected" ).val();
                        $( '#experimentName' ).val(selectedExpName);    //temp hack to fix validation

                        // Set the flag to updating
                        updating = true;

                        console.log(selectedExpName);
                        // retrieve the index of the selected experiment name
                        var idx = $.inArray(selectedExpName, GLOBAL.master_experiments_array['names']);
                        // retrieve the associated experiment ID
                        var exp_id = (idx >= 0) ? parseInt(GLOBAL.master_experiments_array['ids'][idx]) : -1;

                        console.log("EXPERIMENT ID: " + exp_id);

                        if(that.validateExperiment() && exp_id > 0) {
                            that._createExperimentParams();
                            planParams['exp_name'] = selectedExpName;
                            console.log(JSON.stringify(planParams));
                            $.ajax({
                                type: "POST",
                                url: baseURLs.EXPERIMENT_URL,
                                data: {json_str: JSON.stringify(planParams), 'exp_id': exp_id, updateExpParams: 1},
                                dataType: "json"
                            }).done(function(json_msg){
                                create("basic-template", { title: 'Success!', text: 'Experiment parameters updated!' }, { custom: true } );
                            }).fail(function(jqXHR, textStatus, errorThrown){
                                console.log(textStatus + ": " + errorThrown);
                                create("error-template", { title: 'Error!', text: 'There was an error in updating your parameters.' }, { custom: true } );
                                var new_error = new SNError( 4, "There was an error in updating your parameters. Please make sure you entered valid values for each field.", null, $( '#feedbackDisplay'), 0);
                                new_error.displayError();
                            });
                        }
                    });

                    $( '#saveObjectInfo' ).on('click', function(event){
                        addingNew = true;
                        var selectedExpName = $( "#myExperimentSelector option" ).filter( ":selected" ).val();
                        $( '#experimentName' ).val(selectedExpName);    //temp hack to fix validation

                        if (that.validateExperiment('objects')){
                            console.log('validated');
                            that._createExperimentParams();
                            planParams['exp_name'] = selectedExpName;

                            console.log("In save object info...." + selectedExpName);

                            // retrieve the index of the selected experiment name
                            var idx = $.inArray(selectedExpName, GLOBAL.master_experiments_array['names']);
                            // retrieve the associated experiment ID
                            var exp_id = (idx >= 0) ? parseInt(GLOBAL.master_experiments_array['ids'][idx]) : -1;

                            $.ajax({
                                type: "POST",
                                url: baseURLs.EXPERIMENT_URL,
                                data: {json_str: JSON.stringify(planParams), 'exp_id': exp_id, saveObjectInfo: 1},
                                dataType: "json"
                            }).done(function(json_msg){
                                if(json_msg['Success'] === 1) {
                                    create("basic-template", { title: 'Success!', text: 'Object information saved!' }, { custom: true } );
                                    $( '#myExperimentSelector' ).trigger('change');     // hack to make sure all updated information is retrieved into local data structures
                                }
                                else {
                                    create("error-template", { title: 'Error!', text: 'There was an error in saving the object information.' }, { custom: true } );
                                    var new_error = new SNError( 4, "There was an error in saving the object information. Please make sure you entered valid values for each field.", null, $( '#feedbackDisplay'), 0);
                                    new_error.displayError();
                                }
                            }).fail(function(jqXHR, textStatus, errorThrown){
                                console.log(textStatus + ": " + errorThrown);
                                create("error-template", { title: 'Error!', text: 'There was an error in saving the object information.' }, { custom: true } );
                                var new_error = new SNError( 4, "There was an error in saving the object information. Please make sure you entered valid values for each field.", null, $( '#feedbackDisplay'), 0);
                                new_error.displayError();
                            });
                        }
                    });

                    $( "#generatePlan" ).on('click', function(event){
                        if(that.validateScheduler(null, clickedObjText)) {
                            that._createExperimentParams();
                            generatePlanDialog.dialog( 'open' );
                        }
                    });

                    // Set up the Generate Plan dialog, set options, and the event handlers
                    generatePlanDialog.dialog({
                        autoOpen: false,
                        width: 400,
                        maxWidth: 400,
                        height: 300,
                        maxHeight: 300,
                        modal: true,
                        resizable: false,
                        zIndex: 1002
                    }).find( "#gpstrategy" ).change( function(){
                        planParams['strategy'] = parseInt($( "#gpstrategy option" ).filter( ":selected" ).val());
                    }).trigger( "change" )
                    .end()
                    .find( "#gpalgorithm").change( function(){
                        planParams['algorithm'] = parseInt($( "#gpalgorithm option" ).filter( ":selected" ).val());
                    }).trigger( "change" )
                    .end()
                    .find( "#gptraining" ).change( function(){
                        planParams['trainning'] = parseInt($( "#gptraining option" ).filter( ":selected" ).val());
                    }).trigger( "change" )
                    .end()
                    .find( "button" ).button().on('click', function(event){
                                //planParams['user_id'] = that.userID;

                                console.log(JSON.stringify(planParams));
                                /* Use JSON.stringify to serialize the array in order to pass it in the POST AJAX call */
                                $.ajax({
                                    type: "POST",
                                    url: baseURLs.GEN_PLAN_URL,
                                    data: {json_str: JSON.stringify(planParams), state: 0},
                                    dataType: "text"
                                }).done(function(json_msg){
                                                      	json_msg = jQuery.parseJSON(json_msg);
                                    if(!json_msg["error"]) {
                                        $("#viewPlanOverlay").css("display", "none");
                                        $("#viewPlanSupernova > table, #viewPlanSupernova > div:not(:first)").css("visibility", "visible");
                                        if (parseInt($( "#gptraining option" ).filter( ":selected" ).val()) == 0) {
                                            $( "#likePlan, #dislikePlan" ).button("disable");
                                        } else if (parseInt($( "#gptraining option" ).filter( ":selected" ).val()) == 1) {
                                            $( "#likePlan, #dislikePlan" ).button("enable");
                                        }
                                        $( "#likePlan, #dislikePlan" ).off('click').on('click', function(event){
                                            var feedback = 0;
                                            if ($(this).attr("id") === "likePlan")
                                                feedback = 1;

                                            var j_str = '{"local_w": ' + JSON.stringify(json_msg["local_w"]) + ', ';
                                            j_str += '"global_w": ' + JSON.stringify(json_msg["global_w"]) + ', ';
                                            j_str += '"feedback": ' + feedback + ', "user_id": ' + that.userID + '}';
                                            $.ajax({
                                                type: "POST",
                                                url: baseURLs.GEN_PLAN_URL,
                                                data: {json_str: j_str, state: 1},
                                                dataType: "text"
                                            }).done(function(msg){
                                                $( "#tryagainPlan" ).button("enable");
                                                if(feedback === 1)
                                                    create("basic-template", { title: "We're glad you like it!", text: 'Thank you for your feedback.' }, { custom: true } );
                                                else
                                                    create("error-template", { title: "Sorry you don't like it!", text: 'Feel free to \'Try Again\' to generate a new plan with the current data. Hint: Try using different combinations of the \'Strategy\' and \'Algorithm\' in the Scheduler. ' }, { custom: true } );
                                            });
                                            $( "#likePlan, #dislikePlan" ).button("disable");
                                        });

                                        that._setUpViewPlan(json_msg, planParams, json_msg.percentage, json_msg.incomplete, json_msg.miss_deadline);
                                    	create("basic-template", { title: 'Success!', text: 'Plan generated!' }, { custom: true } );
                                    	console.log(json_msg);
                                     } else {
                                         create("error-template", { title: 'Error!', text: 'There was an error in generating your plan. Please try again.' }, { custom: true } );
                                     }

                                }).fail(function(jqXHR, textStatus, errorThrown){
                                    console.log(textStatus, errorThrown);
                                    create("error-template", { title: 'Error!', text: 'There was an error in generating your plan. Please try again.' }, { custom: true } );
                                    var new_error = new SNError( 4, "There was an error in generating your plan. Please try changing the strategy/algorithm used or enter different values for the supplemental information for the objects in your list.", null, $( '#feedbackDisplay'), 0);
                                    new_error.displayError();
                                });

                                generatePlanDialog.dialog("close");
                        });


                    /*********************************/
                    /*   View Plan Initializations   */
                    /*********************************/
                    $( "#tryagainPlan" ).on('click', function(){
                        $.ajax({
                            type: "POST",
                            url: baseURLs.GEN_PLAN_URL,
                            data: {json_str: JSON.stringify(planParams), state: 0},
                            dataType: "json"
                        }).done(function(json_msg){
                            that._setUpViewPlan(json_msg,planParams, json_msg.percentage, json_msg.incomplete, json_msg.miss_deadline);
                            if (parseInt($( "#gptraining option" ).filter( ":selected" ).val()) == 0) {
                                $( "#likePlan, #dislikePlan" ).button("disable");
                            } else if (parseInt($( "#gptraining option" ).filter( ":selected" ).val()) == 1) {
                                $( "#likePlan, #dislikePlan" ).button("enable");
                            }
                        }).fail(function(jqXHR, textStatus, errorThrown){
                            console.log(textStatus, errorThrown);
                        });
                    });

        },

        _createExperimentParams: function() {
            var that = this;
            //planParams = {};    //reset global object that holds all the plan parameters

            planParams['data1'] = [];
            planParams['data2'] = {};
            planParams['data2']['NNights'] = parseInt($.trim($( "#scheduleNumNights" ).val()));
            planParams['data2']['LNights'] = [];

            $.each($('[id^="schNight"]'), function(index){
                var tempObj = {}, half_night = "";
                if($('#halfA' + index).val() !== "" && $('#halfB' + index).val() === "")
                    half_night = $('#halfA' + index).val();
                else if($('#halfA' + index).val() === "" && $('#halfB' + index).val() !== "")
                    half_night = $('#halfB' + index).val();
                tempObj[$(this).val()] = $( '#schNHours' + index ).val() + half_night;  // set the object to have the date as the key and the nights/half night as value

                //push the object into the LNights array
                planParams['data2']['LNights'].push(tempObj);
            });

            var success = false;

                for(var key in GLOBAL.master_objects_array) {
                    if(GLOBAL.master_objects_array.hasOwnProperty(key)) {
                        var moa = GLOBAL.master_objects_array[key], objInfo = [], mainObjInfo = {}, supObjInfo = {};

                            success = true;
                            mainObjInfo['_Name'] = key;
                            mainObjInfo['_Id'] = moa.id;
                            mainObjInfo['_RA'] = parseFloat(moa.ra);
                            mainObjInfo['_Dec'] = parseFloat(moa.dec);
                            mainObjInfo['_Type'] = moa.type;
                            mainObjInfo['_Redshift'] = parseFloat(moa.redshift);
                            mainObjInfo['_Mag'] = parseFloat(moa.mag);
                            mainObjInfo['_B-Peak'] = moa.bpeak;
                            mainObjInfo['_Priority'] = parseFloat(moa.priority);

                            supObjInfo['_obsGap'] = parseInt(moa.obsgap);
                            supObjInfo['_obsTimes'] = parseInt(moa.obstimes);

                            objInfo = [mainObjInfo, supObjInfo];

                            planParams['data1'].push(objInfo);
                    }
                }
            planParams['user_id'] = that.userID;

        },

        /* Reset the entire SNeT extension when logging in and out. */
        clear: function() {
            var that = this;
            // Step 1: Clear out all of the input fields
            $( ".displaySupernovaContent input[type='text']" ).val('');

            // Step 2: Reset the overlays
            that._search_results_flag = false;
            that._view_plan_flag = false;
            that._scheduler_flag = false;
            $("#scheduleObjectViewOverlay").css("display", "block");     //set the display to none
            $("#scheduleObjectView").css("display", "none");           //set the object view's display to block

            // Step 3: Reset all error labels
            $( ".snerror" ).text('');
        },

        /* Reset the styles for error checking textfields. */
        _clearTFCSS: function(element) {
            element.css( {'border-color':'', 'box-shadow':''} );
            if(element.siblings( 'span' ).size() > 0)
                element.siblings( 'span' ).find( 'img' ).css('visibility', 'hidden');
            else
                element.parent().next( 'span' ).find( 'img' ).css('visibility', 'hidden');
        },

        /* Invoked when opening the SNeT dialog. */
        open: function(){
                    $.ui.dialog.prototype.open.apply(this);  //apply the open method on the dialog prototype - the constructor of the ui.dialog widget
                    this.userID = getUserId();  //store the userID as a property of the widget for global access within the widget
                    GLOBAL.userID = (GLOBAL.userID != -1) ? GLOBAL.userID : getUserId();
                    ////////////////////////////////////////////////////
                    //
                    //   INIT FUNCTIONS
                    //
                    ////////////////////////////////////////////////////
                    if(isUserLoggedIn()) {
                        INIT.initLists();
                        INIT.initExperiments();
                    }

                    $(window).trigger("resize.responsive");    //trigger the resize.responsive event whenever the dialog is opened
                    fluidDialog();                             //call the fluidDialog method to set up the window resize events

                    //Set up the initial views
                    var initialTab = this.childList.removeClass("selectedSupernovaTab").eq(0);
                    initialTab.addClass("selectedSupernovaTab");
                    this.progressBar.progressbar({value: 19.8});    // set up the progress bar
                    $( ".displaySupernovaContent" ).hide();  // Hide all tab content initially
                    $("#searchSupernova").fadeIn({duration: 1000});     //fade in the first div content
        },

        /* Function that handles changing the color of validated objects in the scheduler. */
        _objectColorValidation: function(coj, co){
            var that = this;
            if((that.master_objects_array[coj].priority && that.master_objects_array[coj].priority !== " " && that.master_objects_array[coj].priority.length > 0)
                            && (that.master_objects_array[coj].bpeak && that.master_objects_array[coj].bpeak !== " " && that.master_objects_array[coj].bpeak.length > 0)
                            && (that.master_objects_array[coj].obsdur && that.master_objects_array[coj].obsdur !== " " && that.master_objects_array[coj].obsdur.length > 0)
                            && (that.master_objects_array[coj].obsgap && that.master_objects_array[coj].obsgap !== " " && that.master_objects_array[coj].obsgap.length > 0)
                            && (that.master_objects_array[coj].obstimes && that.master_objects_array[coj].obstimes !== " " && that.master_objects_array[coj].obstimes.length > 0))
                        {
                                co.css({"border": "3px solid #039F12"});

                                co.css({'background': "rgb(180,221,180)"}); /* Old browsers */
                                co.css({'background': "-moz-linear-gradient(top, rgba(222,252,225,1) 0%, rgba(198,250,203,1) 12%, rgba(178,248,184,1) 33%, rgba(129,255,140,1) 67%, rgba(111,255,123,1) 100%)"}); /* FF 3.6+ */
                                co.css({'background': "-webkit-gradient(linear, left top, left bottom, color-stop(0%,rgba(222,252,225,1)), color-stop(12%,rgba(198,250,203,1)), color-stop(33%,rgba(178,248,184,1)), color-stop(67%,rgba(129,255,140,1)), color-stop(100%,rgba(111,255,123,1)))"}); /* Safari, Chrome */
                                co.css({'background': "-webkit-linear-gradient(top, rgba(222,252,225,1) 0%,rgba(198,250,203,1) 12%,rgba(178,248,184,1) 33%,rgba(129,255,140,1) 67%,rgba(111,255,123,1) 100%)"}); /* Chrome10+,Safari5.1+ */
                                co.css({'background': "-o-linear-gradient(top, rgba(222,252,225,1) 0%,rgba(198,250,203,1) 12%,rgba(178,248,184,1) 33%,rgba(129,255,140,1) 67%,rgba(111,255,123,1) 100%)"}); /* Opera 11.10+ */
                                co.css({'background': "-ms-linear-gradient(top, rgba(222,252,225,1) 0%,rgba(198,250,203,1) 12%,rgba(178,248,184,1) 33%,rgba(129,255,140,1) 67%,rgba(111,255,123,1) 100%)"}); /* IE10+*/
                                co.css({'background': "linear-gradient(to bottom, rgba(222,252,225,1) 0%,rgba(198,250,203,1) 12%,rgba(178,248,184,1) 33%,rgba(129,255,140,1) 67%,rgba(111,255,123,1) 100%)"}); /* W3C */
                                co.css({'filter': "progid:DXImageTransform.Microsoft.gradient( startColorstr='#b4ddb4', endColorstr='#002400',GradientType=0 )" });

                                that.validateScheduler("save", coj);
                            } else {
                                co.css({"border": "3px solid #F90606"});

                                co.css({'background': "rgb(255,173,145)"}); /* Old browsers */
                                co.css({'background': "-moz-linear-gradient(top,  rgba(255,173,145,1) 0%, rgba(252,135,114,1) 100%)"}); /* FF 3.6+ */
                                co.css({'background': "-webkit-gradient(linear, left top, left bottom, color-stop(0%,rgba(255,173,145,1)), color-stop(100%,rgba(252,135,114,1)))"}); /* Safari, Chrome */
                                co.css({'background': "-webkit-linear-gradient(top,  rgba(255,173,145,1) 0%,rgba(252,135,114,1) 100%)"}); /* Chrome10+,Safari5.1+ */
                                co.css({'background': "-o-linear-gradient(top,  rgba(255,173,145,1) 0%,rgba(252,135,114,1) 100%)"}); /* Opera 11.10+ */
                                co.css({'background': "-ms-linear-gradient(top,  rgba(255,173,145,1) 0%,rgba(252,135,114,1) 100%)"}); /* IE10+*/
                                co.css({'background': "linear-gradient(to bottom,  rgba(255,173,145,1) 0%,rgba(252,135,114,1) 100%)"}); /* W3C */
                                co.css({'filter': "progid:DXImageTransform.Microsoft.gradient( startColorstr='#ffad91', endColorstr='#fc8772',GradientType=0 )" });
                            }
        },

        /* Populates the objects from the user's selected list and sets up event handlers for the objects, the save button, and the supplemental textfields. */
        populateObjects: function(){
                    var that = this, aaData = [], counter = 0;

                    // Retrieve the objects from the array, storing it in a temporary 2D array to pass to the datatables instance below
                    for(var key in GLOBAL.master_objects_array) {
                        var inner_aaData = [];
                        if(GLOBAL.master_objects_array.hasOwnProperty(key)) {
                            var temp_obj = GLOBAL.master_objects_array[key];
                            inner_aaData.push('<a href="#" class="sn_moreinfo">' + key + '</a>', temp_obj.ra, temp_obj.dec, temp_obj.type,
                                //'<input class="numericInput" type="text" id="magtf' + counter + '" size="6" value="' + temp_obj.mag + '"/>',
                                '<input class="numericInput" type="text" id="redshifttf' + counter + '" size="8" value="' + temp_obj.redshift + '"/>',
                                temp_obj.phase,
                                '<input type="text" id="bpeaktf' + counter + '" size="10" placeholder="Choose a date."/>',
                                '<input class="numericInput" type="text" id="obsgaptf' + counter + '" size="8" value="1" placeholder="Enter a value."/>',
                                '<input class="numericInput" type="text" id="obstimestf' + counter + '" size="8" placeholder="Enter a value."/>',
                                '<input class="numericInput" type="text" id="prioritytf' + counter + '" size="6" value="0.5" placeholder="Enter a value."/>');
                        }
                        aaData.push(inner_aaData);
                        counter++;
                    }

                    /* Step 1: Initialize the datatables instance. */
                    scheduleTable = $("#scheduleObjectDisplay").dataTable( {
                            "aoColumnDefs": [
                                { "aTargets": [ 0 ], "sTitle": "Supernovae" },
                                { "aTargets": [ 1 ], "sTitle": "R.A.", "sType": "numeric", "sWidth": "9%" },
                                { "aTargets": [ 2 ], "sTitle": "Dec.", "sType": "numeric", "sWidth": "9%" },
                                { "aTargets": [ 3 ], "sTitle": "Type", "sWidth": "9%" },
                                //{ "aTargets": [ 4 ], "sTitle": "Mag.", "sType": "numeric", "sWidth": "9%" },
                                { "aTargets": [ 4 ], "sTitle": "z", "sType": "numeric", "sWidth": "9%" },
                                { "aTargets": [ 5 ], "sTitle": "Phase", "sWidth": "9%" },
                                { "aTargets": [ 6 ], "sTitle": "B-Peak", "sWidth": "9%" },
                                { "aTargets": [ 7 ], "sTitle": "Gap (days)", "sType": "numeric", "sWidth": "9%" },
                                { "aTargets": [ 8 ], "sTitle": "# times obs.", "sType": "numeric", "sWidth": "9%" },
                                { "aTargets": [ 9 ], "sTitle": "Priority", "sType": "numeric", "sWidth": "9%" }

                            ],
                            "aaData": aaData,
                            "bAutoWidth": false,
                            "bDestroy": true,
                            "bJQueryUI": true,
                            "bPaginate": false,
                            "bLengthChange": false,
                            "bScrollCollapse": true,
                            "oLanguage": { "sSearch": "Filter: " },
                            "sPaginationType": "full_numbers",
                            "sScrollY": $('#scheduleObjectOverview').height() - 150,
                            "sScrollX": "100%",
                            "sScrollXInner": "110%"
                        }).css('width', '');

                // new AutoFill( scheduleTable, {
                //     "aoColumnDefs": [
                //         {
                //             "fnStep": function ( nTd, oPrepped, iDiff, bIncrement, sToken ) {
                //                 console.log(oPrepped.iStart + ", " + iDiff);
                //                 var iReplace = oPrepped.iStart;
                //                 if ( isNaN(iReplace) )
                //                 {
                //                     iReplace = "";
                //                 }
                //                 console.log(oPrepped.sStr.replace( sToken, iReplace+oPrepped.sPostFix ));
                //                 //$('[id^="prioritytf"], [id^="bpeaktf"], [id^="obsdurtf"], [id^="obsgaptf"], [id^="obstimestf"]').trigger('change');
                //                 // if(oPrepped.iStart == 0)
                //                 //     $('#prioritytf' + iDiff).trigger('change');
                //                 // if(oPrepped.iStart == 1)
                //                 //     $('#bpeaktf' + iDiff).trigger('change');
                //                 // if(oPrepped.iStart == 2)
                //                 //     $('#obsdurtf' + iDiff).trigger('change');
                //                 // if(oPrepped.iStart == 3)
                //                 //     $('#obsgaptf' + iDiff).trigger('change');
                //                 // if(oPrepped.iStart == 4)
                //                 //     $('#obstimestf' + iDiff).trigger('change');

                //                 return oPrepped.sStr.replace( sToken, iReplace+oPrepped.sPostFix );
                //             },
                //             "aTargets": [ 7, 8, 9, 10, 11 ]
                //         }
                //     ]
                // });

                // Bind a click handler to the Supernova names
                $( 'a[class="sn_moreinfo"]' ).on('click', function(event){
                    event.preventDefault();
                    var clickedObjText = $(this).text();
                    var curObj = GLOBAL.master_objects_array[clickedObjText];
                    var ret_str = "";   // will hold the stringified HTML to add to the window
                    ret_str += '<p class="mcTitle"><strong>' + clickedObjText + '</strong></p>';

                    console.log(curObj.message);
                    $.each(curObj.message, function(index, elem){
                        ret_str += '<div class="messageContainer hideshow">';
                        ret_str += '<h1><a href="' + this.feed.url + '">' + this.feed.name + '</a></h1>';
                        ret_str += '<h2>Feed Description</h2>';
                        ret_str += '<p>' + this.feed.description + '</p>';
                        ret_str += '<h2>Link</h2>';
                        this.link !== 'Not Available' ? ret_str += '<p><a href="' + this.link + '">' + this.link + '</a></p>' : ret_str += '<p>' + this.link + '</p>';
                        ret_str += '<h2>Message Description</h2>';
                        ret_str += '<p>' + this.description + '</p>';
                        ret_str += '</div>'
                    });

                    var windowName = "More details on SUPERNOVA";
                    var windowSize = "width=800,height=800,scrollbars=yes";
                    var OpenWindow = window.open('', windowName, windowSize);
                    var doc = OpenWindow.document;
                    doc.write(
                        "<html><head>"
                        + '<style type="text/css">'
                        + ".mcTitle{ color: #000000; font-family: Verdana, Verdana, Geneva, sans-serif; font-size: 20px; text-align: center; }"
                        + ".messageContainer{ background: #021833; border-radius: 5px; -moz-border-radius: 5px; font-family: Verdana, Verdana, Geneva, sans-serif; margin: 5px 0; padding: 5px; }"
                        + ".messageContainer h1 a { color:#E69719; font-size:18px; }"
                        + ".messageContainer h2 { color:#CF4A08; font-size:15px; margin-left:10px; }"
                        + ".messageContainer p, .messageContainer p a { color:#FFFFFF; font-size:13px; margin-left:20px; }"
                        + '</style>'
                        + '<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.7.2/jquery.min.js" type="text/javascript"></script> '
                        + "</head><body>" + ret_str
                        + "<script type='text/javascript'>"
                        + " $('.hideshow').on('click', function(){ }); "
                        + "</script>"
                        + "</body></html>"
                    );
                    doc.close();
                    // OpenWindow.document.write(ret_str);
                });

                //set up the datepicker for B-peak
                $('[id^="bpeaktf"]').datepicker({ dateFormat: 'yy-mm-dd' });

                // Set up the change handlers for each supplemental textfield
                /* Handle temporary saving to the master objects array when switching screens, or clicking somewhere else in the application. */

                $('[id^="magtf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].mag = $(this).val().length > 0 ? $(this).val() : "0";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                }).trigger('change');
                $('[id^="redshifttf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].redshift = $(this).val().length > 0 ? $(this).val() : "0";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                }).trigger('change');
                $('[id^="phasetf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].phase = $(this).val().length > 0 ? $(this).val() : "undefine";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                }).trigger('change');


                $('[id^="prioritytf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].priority = $(this).val().length > 0 ? $(this).val() : "0.5";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                }).trigger('change');
                $('[id^="bpeaktf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].bpeak = $(this).val().length > 0 ? $(this).val() : " ";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                });
                $('[id^="obsdurtf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].obsdur = $(this).val().length > 0 ? $(this).val() : " ";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                });
                $('[id^="obsgaptf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].obsgap = $(this).val().length > 0 ? $(this).val() : " ";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                });
                $('[id^="obstimestf"]').on('change', function(event){
                    // get the supernova name based off the changing textfield
                    var clickedObjText = $(this).parents('tr').children(':first').text();
                    GLOBAL.master_objects_array[clickedObjText].obstimes = $(this).val().length > 0 ? $(this).val() : " ";
                    console.log(GLOBAL.master_objects_array);

                    event.stopPropagation();
                });

        },

        /* Utility function for getting all the numbered IDs of the dragged objects in the new list. */
        _retrieveDraggedIDs: function() {
                    var objectIDList = [];
                    for (var y=0, aLen = aDragged.length; y < aLen; ++y) {
                        var substr = aDragged[y].split("_");
                        objectIDList[y] = substr[1] + '';
                    }

                    return objectIDList;
        },

        /* Utility function to retrieve all the user's lists */
        _retrieveUserLists: function(success_callback, failure_callback) {
            var self = this;
            // If the retrieve function is called from the List Management section,
            // retrieve all of the user's lists
            if (self._list_management_flag) {
                $.ajax( {
                    type: "POST",
                    dataType: "json",
                    url: baseURLs.LISTMANAGEMENT_URL,
                    data: { 'retrieveAll': 1 }
                } ).done(success_callback)
                .fail(failure_callback);

                // Reset the list management flag
                self._list_management_flag = false;
            } else {
              /* Make an AJAX request to retrieve all the user's lists */
                $.ajax({
                    type: "POST",
                    dataType: "json",
                    url: baseURLs.LIST_URL,
                    data: { retrieveLists: 1, _userID: GLOBAL.userID }
                }).done(success_callback)
                .fail(failure_callback);
            }

                //cache the user's lists in one array that can be used instead of making multiple calls
        },

        search: function(name, ra, dec, epsilon, browse, adv){
                    var that = this;
                    if(!adv && !browse && !name && ((!ra && ra != 0) || (!dec && dec != 0) || (!epsilon && epsilon != 0))){
                        console.log("ERROR:\n" + name + " and " + ra + " and " + dec + " and " + epsilon);
                        return;
                    }

                    /*  Set the values as empty strings to keep the AJAX call from complaining.  */
                    var asscData = {
                        name: "",
                        _ra: "",
                        _dec: "",
                        _epsilon: "",
                        _browse: "",
                        _adv: {}
                    };

                    if(name){
                        asscData.name = name;
                        that._setUpSearchResults();
                        that._setUpDataTables(1, asscData);
                    } else if(ra && dec && epsilon){
                        asscData._ra = ra;
                        asscData._dec = dec;
                        asscData._epsilon = epsilon;
                        that._setUpSearchResults();
                        that._setUpDataTables(2, asscData);
                    } else if(browse) {
                        asscData._browse = browse;
                        that._setUpSearchResults();
                        that._setUpDataTables(3, asscData);
                    } else if(adv) {
                        asscData._adv = adv;
                        console.log(adv);
                        console.log(JSON.stringify(asscData._adv));
                        that._setUpSearchResults();
                        that._setUpDataTables(4, asscData);
                    }
        },

        _setUpListManagement: function(){
                    var that = this;
                    that._list_management_flag = true;
                    listManagementTable = $("#listManagementDisplay").dataTable({
                                "aoColumnDefs": [
                                    { "aTargets": [ 0 ], "bFilter": false, "bSortable": false, "sWidth": "10px" },
                                    { "aTargets": [ 1 ], "sTitle": "Supernovae", "sWidth": "10%", "sClass": "centerAlign" },
                                    { "aTargets": [ 2 ], "sTitle": "Right Ascension", "sType": "numeric", "sClass": "rightAlign" },
                                    { "aTargets": [ 3 ], "sTitle": "Declination", "sType": "numeric", "sClass": "rightAlign" },
                                    { "aTargets": [ 4 ], "sTitle": "Type", "sClass": "centerAlign" },
                                    { "aTargets": [ 5 ], "sTitle": "Magnitude", "sType": "numeric", "sClass": "centerAlign" },
                                    { "aTargets": [ 6 ], "sTitle": "Redshift", "sType": "numeric", "sClass": "centerAlign" }
//                                    { "aTargets": [ 7 ], "sTitle": "Date Reported", "sClass": "centerAlign" }
                                ],
                                "bJQueryUI": false,
                                "bAutoWidth": false,
                                "bProcessing": true,
                                "bFilter": false,
                                "bLengthChange": true,
                                "oLanguage": { "sSearch": "Filter: " },
                                "sPaginationType": "full_numbers",
                                "sScrollY": 0.9 * ($(window).height() - ($("#supernovae_dialog").parent(".ui-dialog").height() / 2)),
                                "bLengthChange": false,
                                "bDestroy": true
                            });

                    var listManagement_success = function(json_msg){
                        // 2D Global array that will maintain the user's full set of lists - [list_name, list_description]
                        that._userLists = json_msg;
                        listNamesArr = [];
                        var listManIds = [], listManNames = [], listManOwnerIds = [];
                        var curListName = "", owner = "";
                        var $listManListSelector = $( "#listManagementListSelector" );
                        var $listManListSelectorOptions = $( "#listManagementListSelector option");
                        listMan_newListSelector = '<select id="listManagementListSelector">';     //create a new select control to replace the old one

                        // loop through the JSON response and populate the select element with the list names
                        if (GLOBAL.userID == 101) {
                            for (var i=0, listLen = json_msg.length; i < listLen; ++i){
                                curListName = json_msg[i][1];
                                owner = json_msg[i][3];

                                if (curListName === 'A_TEST_CASE')
                                    listMan_newListSelector += '<option selected="selected" value="' + curListName + '">' + curListName + '</option>';
                                else
                                    listMan_newListSelector += '<option value="' + curListName + ' - ' + owner + '">' + curListName + ' - ' + owner + '</option>';
                            }
                        } else {
                            for (var i=0, listLen = json_msg.length; i < listLen; ++i){
                                curListName = json_msg[i][1];
                                owner = json_msg[i][3];
                                listMan_newListSelector += '<option value="' + curListName + ' - ' + owner + '">' + curListName + ' - ' + owner + '</option>';

                                // Add the list ids and names in a separate array - this is only for list management
                                listManIds.push(json_msg[i][0]);
                                listManNames.push(curListName + ' - ' + owner);
                                listManOwnerIds.push(json_msg[i][4]);
                            }
                        }

                        $listManListSelector.replaceWith(listMan_newListSelector);  //replace the select element with the newly created one

                        $.each(json_msg, function(index){
                            listNamesArr.push(json_msg[index][0]);  //populate the global array containing all of the list names
                        });

                        /* Set up the onchange handler for the list selector to retrieve SN objects */
                        $( "#listManagementListSelector" ).on('change', function() {
                            listManagementTable.fnClearTable();
                            var selectedList = $( "#listManagementListSelector option" ).filter(":selected").val();   //obtain the selected option
                            var index = $.inArray(selectedList, listManNames);
                            var selectedListID = listManIds[index];
                            console.log("LIST ID: " + selectedListID);
                            /** Set the current list to the one selected - retrieve it from global array of lists. **/
                            //that._cur_user_list = json_msg[$("#listManagementListSelector")[0].selectedIndex];
                            //aDragged = [];  //reset the dragged array when switching lists

                            //if(selectedList !== "New List") {
                                //that.userListIDs = [];
                                //that._clicked_other_list = true;
                                /* Make an AJAX POST request to retrieve all the objects in the selected list */
                                $.ajax({
                                    type: "POST",
                                    dataType: "json",
                                    url: baseURLs.LIST_URL,
                                    data: { objects: 1, _listMan: 1, _selectListID: selectedListID, _userID: GLOBAL.userID }
                                }).done(function(json_msg){
                                    var res = json_msg;
                                    /* For each object returned from the AJAX call, retrieve the info, store it in a 1D array and add it to the new list table. */
                                    $.each(res['0'], function(){
                                        var object_info = [];
                                        if ( listManOwnerIds[index] == GLOBAL.userID ) {
                                            object_info[0] = '<a class="remove_new_list" href="#"><span class="ui-icon ui-icon-circle-close"></span></a>';
                                            object_info[1] = this.object_name.split(",")[0];
                                            object_info[2] = this.object_ra.toFixed(5);
                                            object_info[3] = this.object_dec.toFixed(5);
                                            object_info[4] = this.object_type;
                                            object_info[5] = this.object_mag;
                                            object_info[6] = this.object_redshift;
                                            object_info[7] = this.object_phase;
                                            $( '#lptext' ).text('Read/Write');
                                            $( '#renameLMList, #deleteLMList, #updateLMList' ).button('option', 'disabled', false);
                                        } else {
                                            object_info[0] = '<a class="get_row_info" href="#"><span class="ui-icon ui-icon-info"></span></a>';
                                            object_info[1] = this.object_name.split(",")[0];
                                            object_info[2] = this.object_ra.toFixed(5);
                                            object_info[3] = this.object_dec.toFixed(5);
                                            object_info[4] = this.object_type;
                                            object_info[5] = this.object_mag;
                                            object_info[6] = this.object_redshift;
                                            object_info[7] = this.object_phase;
                                            $( '#lptext' ).text('Read');
                                            $( '#renameLMList, #deleteLMList, #updateLMList' ).button('option', 'disabled', true);
                                        }

                                        //that.userListIDs.push(this.object_id + '');

                                        listManagementTable.fnAddData(object_info);
                                        //console.log(listManagementTable.fnGetData());
                                    });
                                    // con_drag_and_user_lists = [];
                                    // $.each(that.userListIDs, function(index){
                                    //     con_drag_and_user_lists.push("row_" + that.userListIDs[index]); //add the objects from the list selected first
                                    // });
                                    // con_drag_and_user_lists = arrayUnique(con_drag_and_user_lists.concat(aDragged)); // concatenate the dragged objects too
                                    // console.log(con_drag_and_user_lists);
                                });
                            // } else {
                            //     con_drag_and_user_lists = [];     // reset the combined lists when switching back to a new list
                            //     $( '#updateList' ).button("option", "disabled", true);  // disable the update button since you can't update a new list
                            //     /* Restore the newly dragged items if the user goes back to their newly created list. */
                            //     if(that.tempNewListCopy != null){
                            //         newListTable.fnAddData(that.tempNewListCopy);
                            //     }
                            // }

                        }).trigger( "change" );

                    };

                    var listManagement_failure = function(jqXHR, textStatus, errorThrown){
                            console.log(textStatus + ", " + errorThrown);
                    };

                    /* Call the private utility method that retrieves all the user's lists and use the above success and failure callbacks. */
                    that._retrieveUserLists(listManagement_success, listManagement_failure);
        },

        _setUpSearchResults: function(listName){
                    var that = this;

                    that._search_results_flag = true;
                    $("#searchResultsOverlay").css("display", "none");
                    $("#searchResultsSupernova").find("> div:not(:first)").css("display", "block");

                    var searchResults_success = function(json_msg){
                        // 2D Global array that will maintain the user's full set of lists - [list_name, list_description]
                        that._userLists = json_msg;
                        listNamesArr = [];
                        $searchResListSelector = $( "#searchResultsListSelector" );
                        $searchResListSelectorOptions = $( "#searchResultsListSelector option");
                        search_newListSelector = '<select id="searchResultsListSelector">';     //create a new select control to replace the old one
                        for (var i=0, listLen = json_msg.length; i < listLen; ++i){
                            var curList = json_msg[i][1];
                            search_newListSelector += '<option value="' + curList + '">' + curList + '</option>';
                        }

                        search_newListSelector += '<option value="New List">New List</option></select>';
                        $searchResListSelector.replaceWith(search_newListSelector);

                        $.each(json_msg, function(index){
                            listNamesArr.push(json_msg[index][0]);  //populate the global array containing all of the list names
                        });

                        if(!that._cur_user_list)
                            $( "#searchResultsListSelector > option:last" ).attr("selected", "selected");
                        else {
                            var listNameIndex = 0;
                            if(listName)
                                listNameIndex = $.inArray(listName, listNamesArr);
                            else
                                listNameIndex = $.inArray(that._cur_user_list[0], listNamesArr);
                            $( "#searchResultsListSelector option").eq(listNameIndex).attr("selected", "selected");
                        }

                        /* Set up the onchange handler for the list selector to retrieve SN objects */
                        $( "#searchResultsListSelector" ).on('change', function() {
                            newListTable.fnClearTable();
                            var selectedList = $( "#searchResultsListSelector option" ).filter(":selected").val();   //obtain the selected option
                            var selectedListID = GLOBAL.master_lists_array['ids'][$.inArray(selectedList, GLOBAL.master_lists_array['names'])];
                            /** Set the current list to the one selected - retrieve it from global array of lists. **/
                            that._cur_user_list = json_msg[$("#searchResultsListSelector")[0].selectedIndex];
                            aDragged = [];  //reset the dragged array when switching lists

                            if(selectedList !== "New List") {
                                that.userListIDs = [];
                                $( '#updateList' ).button("option", "disabled", false);
                                that._clicked_other_list = true;
                                /* Make an AJAX POST request to retrieve all the objects in the selected list */
                                $.ajax({
                                    type: "POST",
                                    dataType: "json",
                                    url: baseURLs.LIST_URL,
                                    data: { objects: 1, _selectListID: selectedListID, _userID: that.userID }
                                }).done(function(json_msg){
                                    var res = json_msg;
                                    /* For each object returned from the AJAX call, retrieve the info, store it in a 1D array and add it to the new list table. */
                                    $.each(res['0'], function(){
                                        object_info = [];
                                        object_info[0] = '<a class="remove_new_list" href="#"><span class="ui-icon ui-icon-circle-close"></span></a>';
                                        object_info[1] = this.object_name.split(",")[0];
                                        object_info[2] = this.object_ra;
                                        object_info[3] = this.object_dec;
                                        object_info[4] = this.object_type;
                                        object_info[5] = this.object_mag;
                                        object_info[6] = this.object_redshift;
                                        object_info[7] = this.object_phase;

                                        that.userListIDs.push(this.object_id + '');

                                        newListTable.fnAddData(object_info);
                                    });
                                    //console.log(that.userListIDs);
                                    con_drag_and_user_lists = [];
                                    $.each(that.userListIDs, function(index){
                                        con_drag_and_user_lists.push("row_" + that.userListIDs[index]); //add the objects from the list selected first
                                    });
                                    con_drag_and_user_lists = arrayUnique(con_drag_and_user_lists.concat(aDragged)); // concatenate the dragged objects too
                                    console.log(con_drag_and_user_lists);
                                });
                            } else {
                                con_drag_and_user_lists = [];     // reset the combined lists when switching back to a new list
                                $( '#updateList' ).button("option", "disabled", true);  // disable the update button since you can't update a new list
                                /* Restore the newly dragged items if the user goes back to their newly created list. */
                                if(that.tempNewListCopy != null){
                                    newListTable.fnAddData(that.tempNewListCopy);
                                }
                            }
                            oTable.fnDraw();    // redraw the table

                        });

                    };

                    var searchResults_failure = function(jqXHR, textStatus, errorThrown){
                            console.log(textStatus + ", " + errorThrown);
                    };

                    /* Call the private utility method that retrieves all the user's lists and use the above success and failure callbacks. */
                    that._retrieveUserLists(searchResults_success, searchResults_failure);
        },

        _setUpScheduler: function(){
                    var selectedList;
                    var that = this;
                    $scheduleObjectDetailViewList = $( "#scheduleObjectView > div div:first ul:first" );
                    $schlistSelector = $( "#scheduleListSelector" );
                    that._scheduler_flag = true;


                    var schedulerList_success = function(json_msg){
                        // 2D array
                        that._userLists = json_msg;
                        newListSelector = '<select id="scheduleListSelector">';    //create a new select to replace the old one

                        for (var i=0, listLen = json_msg.length; i < listLen; ++i){
                            if (i == 0) {
                                newListSelector += '<option selected="selected" value="' + json_msg[i][0] + '">' + json_msg[i][0] + '</option>';
                            } else {
                                newListSelector += '<option value="' + json_msg[i][0] + '">' + json_msg[i][0] + '</option>';
                            }
                        }

                        newListSelector += '</select>';
                        $schlistSelector.replaceWith(newListSelector);  //replace the old select control
                        console.log("done. Now setting up change handler...");

                        /* Set up the onchange handler for the list selector to retrieve SN objects */
                        $( "#scheduleListSelector" ).on('change', function() {
                                var selectedList = $( "#scheduleListSelector option" ).filter(":selected").val();   //obtain the selected option
                                console.log(selectedList);

                                /* Make an AJAX POST request to retrieve all the objects in the selected list */
                                $.ajax({
                                    type: "POST",
                                    dataType: "json",
                                    url: baseURLs.LIST_URL,
                                    data: { objects: 1, _selectList: selectedList, _userID: that.userID }
                                }).done(function(json_msg){
                                    // cache the json returned in a private variable in this closure
                                    var res = json_msg;

                                        that.master_objects_array = [];     //reset the array each time a new list is selected - the master array only contains the objects of the currently viewed list
                                        $.each(res, function(){
                                            object_info = {};
                                            object_info.id = this.object_id;
                                            object_info.name = this.object_name.split(",").splice(1);
                                            object_info.ra = this.object_ra.toFixed(5);
                                            object_info.dec = this.object_dec.toFixed(5);
                                            object_info.type = this.object_type;
                                            object_info.mag = this.object_mag;
                                            object_info.redshift = this.object_redshift;
                                            object_info.phase = this.object_phase;

                                            that.master_objects_array[this.object_name.split(",")[0]] = object_info;
                                        });
                                        that.populateObjects();
                                        scheduleTable.fnAdjustColumnSizing();
                                    // remove any tooltips still around
                                    $(".qtip").remove();
                                });
                        }).trigger( "change" );
                    };

                    var schedulerList_failure = function(jqXHR, textStatus, errorThrown){
                        console.log(textStatus + ", " + errorThrown);
                    };

                    scheduleTable.fnAdjustColumnSizing();

                    //that._retrieveUserLists(schedulerList_success, schedulerList_failure);

                    /* Set up the multiselect datepicker */
                    // $( "#scheduleObsDates" ).multiDatesPicker({
                    //     numberOfMonths: 2,
                    //     dateFormat: 'yy-mm-dd'
                    // });
                    // // Maintain array of dates
                    // var dates = new Array();
                    // var clickedDate = false;
                    // function addDate(date) {if (jQuery.inArray(date, dates) < 0) dates.push(date);}
                    // function removeDate(index) {dates.splice(index, 1);}

                    // // Adds a date if we don't have it yet, else remove it
                    // var addOrRemoveDate = function(date)
                    // {
                    //     clickedDate = true;
                    //   var index = jQuery.inArray(date, dates);
                    //   if (index >= 0)
                    //     removeDate(index);
                    //   else
                    //     addDate(date);
                    // };

                    // // Takes a 1-digit number and inserts a zero before it
                    // var padNumber = function(number)
                    // {
                    //   var ret = new String(number);
                    //   if (ret.length == 1)
                    //     ret = "0" + ret;
                    //   return ret;
                    // };

                    // $("#scheduleObsDates").datepicker({
                    //             onSelect: function(dateText, inst) {
                    //                 addOrRemoveDate(dateText);
                    //                 //console.log("DATES: " + dates);
                    //                 $("#scheduleObsDates").val(dates.join(", "));
                    //                 $(this).data('datepicker').inline = true;
                    //                 $(this).datepicker("refresh");
                    //             },
                    //             dateFormat: 'yy-mm-dd',
                    //             beforeShowDay: function (date){
                    //                 if(clickedDate) {
                    //                     dates = ($.trim($( "#scheduleObsDates" ).val()).split(/,\s*/))[0] === "" ? [] : dates;
                    //                 }
                    //                 var gotDate = $.inArray($.datepicker.formatDate($(this).datepicker('option', 'dateFormat'), date), dates);
                    //                 if (gotDate >= 0) {
                    //                     // Enable date so it can be deselected. Set style to be highlighted
                    //                     return [true,"ui-state-highlight"];
                    //                 }
                    //                 // Dates not in the array are left enabled, but with no extra style
                    //                 return [true, ""];
                    //             },
                    //             numberOfMonths: 3,
                    //             onClose: function(dateText, inst) {
                    //                 $(this).data('datepicker').inline = false;
                    //             }
                    // });

        },

               //TODO
        _setUpViewPlan: function(json_msg, json_input, percentage_hit, incomplete, miss_deadline){
                var that = this;
                that._scheduler_flag = true;
                var obj_missed_deadline = "";
                var obj_incomplete = "";

                that._view_plan_flag = true;
                cur_color_list = [];

                $.each(miss_deadline, function(obj_id, obj){
                    obj_missed_deadline += obj.name + ", ";
                });
                obj_missed_deadline = obj_missed_deadline.substring(0, obj_missed_deadline.length - 2);    // remove trailing space and comma

                $.each(incomplete, function(obj_id, obj){
                    obj_incomplete += obj.name + ", ";
                });
                obj_incomplete = obj_incomplete.substring(0, obj_incomplete.length - 2);   // remove trailing space and comma

                if(obj_missed_deadline === "")
                	obj_missed_deadline = "N/A";

                if(obj_incomplete === "")
                	obj_incomplete = "N/A";
                /***********************************Added by Nikhil*****************************************/
			  $("#viewPlanDisplay").find("thead").children().remove();
			  $("#viewPlanDisplay").find("tbody").children().remove();

              NightLength = new Object();
              for(i=0;i<json_input.data2.LNights.length;i++){
               		var list = json_input.data2.LNights[i];
               		for(var l in list){
               			var indexA = list[l].indexOf('a');
               			var indexB = list[l].indexOf('b');
               			if(indexA>-1 || indexB>-1){
               				NightLength[l]=parseInt(list[l].substring(0,list[l].length-1)) / 2;
               			}else{
               				NightLength[l]=parseInt(list[l]);
				}
			}
               }

                var data = new Object();

				var plan = json_msg["plan"];
				var sPlan = new Object();

				dateList = new Array();
				for ( var daySchedule in plan) {
					dateList.push(daySchedule);
				}
				dateList.sort(); //Specified observation dates in order

				//Define redshift and bpeak for each supernova
				var supernova_info = new Object(); // {"LSQ13baa":{"z":0.082,"bpeak":"2013-10-22"}}

				for(i=0;i<json_input.data1.length;i++){
					s0 = (json_input.data1[i])[0];
					s1 = (json_input.data1[i])[1];
					supernova_info[s0["_Name"]]={"z":s0["_Redshift"], "bpeak":s0["_B-Peak"], "obsTimes":s1["_obsTimes"]};
				}

				for (i = 0; i < dateList.length; i++) {
					everydaySch = plan[dateList[i]];
					for (j = 0; j < everydaySch.length; j++) {
						if (sPlan[everydaySch[j]["name"]] != null) {
							sPlan[everydaySch[j]["name"]]
									.push([
											dateList[i],
											everydaySch[j]["flag"],
											everydaySch[j]["duration"] ]);
						} else {
							sPlan[everydaySch[j]["name"]] = new Array();
							sPlan[everydaySch[j]["name"]]
									.push([
											dateList[i],
											everydaySch[j]["flag"],
											everydaySch[j]["duration"] ]);
						}
					}
				};

				data.aoColumns = [ {"sTitle" : "Name"},
				  { "sTitle": "z" },
				   { "sTitle": "B-Peak" }
				];
				var result = generateTable(supernova_info, sPlan, data.aoColumns);
				data.aaData = result.plan;
				data.aoColumns = result.header;

				that._autoSwitch("100%", 4);
				planTable = $("#viewPlanDisplay")
						.dataTable(
								{
									"aaData" : data.aaData,
									"aoColumns" : data.aoColumns,
									"bSort" : false,
									"bDestroy": true,
									"bPaginate": false,
									"bLengthChange": false,
									"bFilter": false
								//	"sScrollY":"300px",
								//	"sScrollX": "100%"
								}).css('width', '');

				$("#viewPlanDisplay").css({"visibility" : "visible"});

    /**********************************End by Nikhil********************************************/

				$( "#planFeedback" ).empty().append("<p>Percentage of objects fully scheduled: <span>" + percentage_hit + "%</span></p>")
				    .append("<br/><p>Objects that missed their deadline: <span>" + obj_missed_deadline + "</span></p>")
				    .append("<br/><p>Objects that are incomplete: <span>" + obj_incomplete + "</span></p>");

				//$( "#tryagainPlan" ).button("disable");

        },

        _setSearchClickHandlers: function(selectedSearchIndex){
            // Bind the click handlers to the search buttons - make sure to use the widget instance to call the private methods
                        var that = this;
                        console.log("Setting... " + selectedSearchIndex);

                    switch(selectedSearchIndex) {
                        // Browse All
                        case 0:
                            //if( $('#browse_all').is(':checked')){
                                $('#searchsupernovas').off('click.RADecSearch click.nameSearch click.advSearch click.browseAll');
                                $('#searchsupernovas').on('click.browseAll', function(event){
                                    if(that.validateSearch(0)) {
                                        that._clean(0);
                                        that.search(null, null, null, null, "checked");
                                        that._autoSwitch("39.6%", 1);
                                    }
                                });
                            //}
                            break;
                        // Search by Name
                        case 1:
                            $('#searchsupernovas').off('click.RADecSearch click.browseAll click.advSearch click.nameSearch');
                            $('#searchsupernovas').on('click.nameSearch', function(event){
                                that._clean(1);
                                var the_name = String($("#obj_name").val());
                                console.log("The name: " + the_name);
                                if(that.validateSearch(1)) {
                                    that.search(the_name, null, null, null, null);    //perform the search
                                    //switch screens after searching
                                    that._autoSwitch("39.6%", 1);
                                }
                            });
                            break;
                        // Search by R.A. and Dec.
                        case 2:
                            $('#searchsupernovas').off('click.nameSearch click.browseAll click.advSearch click.RADecSearch');
                            $('#searchsupernovas').on('click.RADecSearch', function(){
                                that._clean(2);
                                var the_ra = parseFloat($("#obj_ra").val());
                                var the_dec = parseFloat($("#obj_dec").val());
                                var the_eplison = parseFloat($("#obj_epsilon").val());
                                console.log("The RA: " + the_ra + " the dec: " + the_dec + " the_eplison " + the_eplison);
                                if(that.validateSearch(2)) {
                                    that.search(null, the_ra, the_dec, the_eplison, null);

                                    that._autoSwitch("39.6%", 1);
                                }
                            });
                            break;
                        // Advanced Search
                        case 3:
                            $('#searchsupernovas').off('click.nameSearch click.browseAll click.RADecSearch click.advSearch');
                            $('#searchsupernovas').on('click.advSearch', function(){
                                that._clean(3);
                                if(that.validateSearch(3)) {
                                    that.search(null, null, null, null, null, advOptions);

                                    that._autoSwitch("39.6%", 1);
                                }
                            });
                            break;
                    }
        },

        _setUpDataTables: function(searchType, asscData) {
                    var that = this;
                    if (typeof searchType === "number"){

                        var sortArray = [that._supernovaOrderBy, that._supernovaSortOrder];
                        aSelected = []; //store the selected items from the table for server-side processing

                        var calcHeight = (($(window).height() - $("#supernovae_dialog").parent(".ui-dialog").height()) / 2);
                        //console.log("HEIGHT: " + calcHeight);

                        oTable = $("#searchResultsDisplay").dataTable( {
                            "aoColumnDefs": [
                                { "aTargets": [ 0 ], "mDataProp": "names.0", "sTitle": "Supernovae" },
                                { "aTargets": [ 1 ], "mDataProp": "ra", "sTitle": "Right Ascension", "sType": "numeric"},
                                { "aTargets": [ 2 ], "mDataProp": "dec", "sTitle": "Declination", "sType": "numeric" },
                                { "aTargets": [ 3 ], "mDataProp": "miscs.0.type", "sTitle": "Type" },
                                { "aTargets": [ 4 ], "mDataProp": "miscs.0.disc_mag", "sTitle": "Magnitude", "sType": "numeric" },
                                { "aTargets": [ 5 ], "mDataProp": "miscs.0.redshift", "sTitle": "Redshift", "sType": "numeric" },
                                { "aTargets": [ 6 ], "mDataProp": "messages.0.update_time", "sTitle": "Date Reported" }
                            ],
                            "aaSorting": [sortArray],   //aaSorting is an array of arrays
                            "bAutoWidth": false,
                            "bJQueryUI": false,
                            "bDeferRender": true,
                            "oLanguage": { "sSearch": "Filter: " },
                            "sPaginationType": "full_numbers",
                            "sScrollY": 0.8 * ($(window).height() - ($("#supernovae_dialog").parent(".ui-dialog").height() / 2)),
                            "sScrollX": "100%",
                            "bLengthChange": true,
                            "bProcessing": false,
                            "bServerSide": true,
                            "bDestroy": true,
                            "sAjaxSource": baseURLs.QUERY_URL,
                            "sServerMethod": "POST",
                            "fnServerParams": function( aoData ) {
                                aoData.push( { "name": "search", "value": searchType }, { "name": "_name", "value": asscData.name },
                                    { "name": "_ra", "value": asscData._ra }, { "name": "_dec", "value": asscData._dec }, { "name": "_epsilon", "value": asscData._epsilon },
                                    { "name": "_browse", "value": asscData._browse }, { "name": "offset", "value": "all" }, { "name": "limit", "value": "all" },
                                    { "name": "_adv", "value": JSON.stringify(asscData._adv) }, { "name": "orderby", "value": "unique_id" }, { "name": "sort", "value": "DESC" } );
                            },
                            "fnServerData": function( sSource, aoData, fnCallback, oSettings ) {
                                oSettings.jqXHR = $.ajax({
                                    "dataType": "json",
                                    "type": "POST",
                                    "url": sSource,
                                    "data": aoData,
                                    "success": fnCallback
                                }).fail(function(jqXHR, textStatus, errorThrown) {
                                    console.log(textStatus, errorThrown); // use alert() if you prefer
                                }).done(function(json_msg){
                                     $("#searchResultsDisplay tbody > tr").draggable({
                                        cursor: "move",
                                        // helper: function(e, tr)
                                        // {
                                        //     var $originals = tr.children();
                                        //     var $helper = tr.clone();
                                        //     $helper.children().each(function(index) {
                                        //       // Set helper cell sizes to match the original sizes
                                        //       $(this).width($originals.eq(index).width());
                                        //     });
                                        //     return $helper;
                                        // },
                                        helper: function(){
                                            var selectedSupernovas = $('.row_selected');        //get the selected rows
                                            if (selectedSupernovas.length === 0) {              //check to see if there are selected rows or not
                                                selectedSupernovas = $(this);                   //if not, then set the selected one to the one being dragged
                                            }
                                            var container = $('<div/>').attr('id', 'draggingContainer');    //create a div container to hold the selected objects
                                            container.append(selectedSupernovas.clone());                   //append clones of the objects to the container
                                            return container;                                               //return the helper
                                        },
                                        containment: "#supernovae_dialog",
                                        opacity: 0.5,
                                        scroll: false,
                                        revert: "invalid",
                                        cancel: ".row_dragged",     //cancel the rows that have already been dragged
                                        start: function(event, ui){

                                        }
                                    }).on('click', function () {
                                            var id = this.id;
                                            var index = $.inArray(id, aSelected);

                                            if ( index === -1 ) {
                                                aSelected.push( id );
                                            } else {
                                                aSelected.splice( index, 1 );
                                            }

                                            $(this).toggleClass('row_selected');
                                        });
                                });
                            },
                            "fnRowCallback": function( nRow, aData, iDisplayIndex ) {
                                if ( $.inArray(aData.DT_RowId, aSelected) !== -1 ) {
                                    $(nRow).addClass('row_selected');
                                }
                                if ( $.inArray(aData.DT_RowId, con_drag_and_user_lists) !== -1 ) {
                                    $(nRow).addClass('row_dragged');
                                }
                            }
                        });


                        newListTable = $("#newListDisplay").dataTable({
                            "aoColumnDefs": [
                                { "aTargets": [ 0 ], "bFilter": false, "bSortable": false, "sWidth": "10px" },
                                { "aTargets": [ 1 ], "sTitle": "Supernovae", "sWidth": "10%" },
                                { "aTargets": [ 2 ], "sTitle": "Right Ascension", "sType": "numeric" },
                                { "aTargets": [ 3 ], "sTitle": "Declination", "sType": "numeric" },
                                { "aTargets": [ 4 ], "sTitle": "Type" },
                                { "aTargets": [ 5 ], "sTitle": "Magnitude", "sType": "numeric" },
                                { "aTargets": [ 6 ], "sTitle": "Redshift", "sType": "numeric" },
                                { "aTargets": [ 7 ], "sTitle": "Date Reported" }
                            ],
                            "bJQueryUI": false,
                            "bAutoWidth": false,
                            "bProcessing": true,
                            "oLanguage": { "sSearch": "Filter: " },
                            "sPaginationType": "full_numbers",
                            "sScrollY": 0.8 * ($(window).height() - ($("#supernovae_dialog").parent(".ui-dialog").height() / 2)),
                            "sScrollX": "100%",
                            "bLengthChange": false,
                            "bDestroy": true
                        });

                        $("#supernovaNewListDisplay")
                            //.css("height", 0.85 * ($(window).height() - ($("#supernovae_dialog").parent(".ui-dialog").height() / 2)))
                            .droppable({
                                accept: "#searchResultsDisplay tbody > tr",
                                activeClass: "ui-droppable-active",
                                hoverClass: "ui-droppable-hover",
                                tolerance: "pointer",
                                drop: function(event, ui){
                                    var addedData = [], i=0;
                                    var draggableIDs = [];

                                    $.each(ui.helper.children(), function(index, value){    //iterate through each of the helper's children (the tr's)
                                        var innerAddedData = [], k=1;
                                        innerAddedData[0] = '<a class="remove_new_list" href="#"><span class="ui-icon ui-icon-circle-close"></span></a>';
                                        $(this).children('td').each(function() {            //iterate through the td's in the table rows to retrieve the data
                                            innerAddedData[k] = $(this).text();             //add each row cell's text into a new array
                                            k++;
                                        });
                                        addedData[i] = innerAddedData;                      //set the arrays in the 2D array of addedData
                                        draggableIDs[i] = $(this).attr( "id" );             //retrieve the id of the dragged row to add/remove classes and enable/disable dragging
                                        i++;
                                    });

                                    newListTable.fnAddData(addedData);          //update the new list with the dragged rows
                                    if(!that._clicked_other_list)   // if an existing list has NOT been clicked...
                                        that.tempNewListCopy = newListTable.fnGetData();    // set the temp copy to the current data in the new list table

                                    for (var i = 0, len = draggableIDs.length; i < len; ++i) {
                                        $("#" + draggableIDs[i]).off('click').removeClass("row_selected").addClass("row_dragged").draggable("disable");

                                        /* Preferred method */
                                        aSelected.splice( $.inArray(draggableIDs[i], aSelected), 1);

                                        var draggedIndex = $.inArray(draggableIDs[i], aDragged);
                                        if ( draggedIndex === -1 ) {
                                            aDragged.push( draggableIDs[i] );
                                            con_drag_and_user_lists.push( draggableIDs[i] );
                                        } else {
                                            aDragged.splice( draggedIndex, 1 );
                                        }
                                    }

                                }
                            });


                        $(window).resize(function(){
                            $('div.dataTables_scrollBody').css('height', 0.8 * ($(window).height() - ($("#supernovae_dialog").parent(".ui-dialog").height() / 2)));
                            oTable.fnAdjustColumnSizing();
                        });
                    }
        },

        // Set up the selected experiment parameters
        // Format: {"num_nights":2,"nights":[{"2013-10-17":{"hours":"4","halfab":"a"}},{"2013-10-18":{"hours":"5","halfab":""}}]}
        setUpExperiment: function(json_msg, type) {
            var supFields = $( '[id^="bpeaktf"], [id^="obsgaptf"], [id^="obstimestf"], [id^="prioritytf"]' );
            if (type && typeof type === 'string' && type === 'objects') {
                objectsInvolved = false;
                 // Initial wipe of the textfields before populating them (if they are available)
                var supFields = $( '[id^="bpeaktf"], [id^="obsgaptf"], [id^="obstimestf"], [id^="prioritytf"]' );
                $.each(supFields, function(index){
                    $(this).val('');
                });
                // Enter the object information, if applicable
                console.log(json_msg);
                var objects = json_msg;
                if(objects.length > 0){
                    var obj_counter = 0;
                    $.each(objects, function(index, elem){
                        var cur_elem = elem;
                        $.each($('[id^="bpeaktf"]'), function(sindex) {
                            var curElemText = $(this).parents('tr').children(':first').text();
                            // console.log("CUR ELEM TEXT " + curElemText);
                            if (GLOBAL.master_objects_array[curElemText] && (GLOBAL.master_objects_array[curElemText].id == parseInt(cur_elem['unique_id']))) {
                                objectsInvolved = true;
                                console.log("SINDEX: " + sindex + " OBJ_COUNTER: " + obj_counter);

                                $( '#bpeaktf' + obj_counter ).val(cur_elem['b_peak']);
                                GLOBAL.master_objects_array[curElemText].bpeak = cur_elem['b_peak'];
                                $( '#obsgaptf' + obj_counter ).val(cur_elem['obs_gap']);
                                GLOBAL.master_objects_array[curElemText].obsgap = cur_elem['obs_gap'];
                                $( '#obstimestf' + obj_counter ).val(cur_elem['obs_times']);
                                GLOBAL.master_objects_array[curElemText].obstimes = cur_elem['obs_times'];
                                $( '#prioritytf' + obj_counter ).val(cur_elem['priority']);
                                GLOBAL.master_objects_array[curElemText].priority = cur_elem['priority'];

                                obj_counter++;
                            }
                        });
                    });
                    // Reset the values if all of the fields haven't been filled out
                    if($('[id^="bpeaktf"]').val() === '') { objectsInvolved = false; }
                } else {
                    // If there were no objects, populate the fields with the master_objects_array values
                    var index = 0;
                    for(var key in GLOBAL.master_objects_array) {
                        if(GLOBAL.master_objects_array.hasOwnProperty(key)) {
                            $( '#bpeaktf' + index ).val(GLOBAL.master_objects_array[key].bpeak);
                            $( '#obsgaptf' + index ).val(GLOBAL.master_objects_array[key].obsgap);
                            $( '#obstimestf' + index ).val(GLOBAL.master_objects_array[key].obstimes);
                            $( '#prioritytf' + index ).val(GLOBAL.master_objects_array[key].priority);
                        }
                        index++;
                    }
                }

                // Final check to see if the objects were populated by the experiment or not
                if (!objectsInvolved) {  // if setupexperiment with objects is not called right before this, reset all the supp values
                    $.each(supFields, function(index){ $(this).val(''); });
                    // Populate the default observation gap as 1
                    $.each($('[id^="obsgaptf"]'), function(){ $(this).val('1'); });
                    // Populate initial values of priority with 0.5
                    $.each($('[id^="prioritytf"]'), function(){ $(this).val('0.5'); });
                }
            } else {
                console.log("OBJ INVOLVED: " + objectsInvolved);
                 // Initial check of textfields to see if they are populated or not
                if (!objectsInvolved) {  // if setupexperiment with objects is not called right before this, reset all the supp values
                    $.each(supFields, function(index){
                        $(this).val('');
                    });
                    // Populate the default observation gap as 1
                    $.each($('[id^="obsgaptf"]'), function(){
                        $(this).val('1');
                    });
                    // Populate initial values of priority with 0.5
                    $.each($('[id^="prioritytf"]'), function(){
                        $(this).val('0.5');
                    });
                }
                $( '#updateExperiment' ).button('option', 'disabled', false);
                // Manually trigger a keyup for the number of nights textfield with the num_nights as input value
                //console.log("NUM NIGHTS: " + json_msg['num_nights']);
                $( '#scheduleNumNights' ).val(json_msg['num_nights']);
                $( '#scheduleNumNights' ).trigger('keyup');
                // if( json_msg['num_nights'] !== '')
                //     GLOBAL.experimentSetUp = true;

                // Enter the nights
                var nights = json_msg['nights'] || {};
                console.log(nights);
                $.each(nights, function(index, elem){
                    var key = $.map(elem, function(item, key) {
                        return key;
                    });
                    $( '#schNight' + index ).val(key);
                    $( '#schNHours' + index ).val(elem[key]['hours']);
                    if(elem[key]['halfab'] === "a"){
                        $( '#halfA' + index ).prop('checked', true);
                        $( '#halfA' + index ).attr('value', 'a');
                        aChecked = true;
                    } else if(elem[key]['halfab'] === "b"){
                        $( '#halfB' + index ).prop('checked', true);
                        $( '#halfB' + index ).attr('value', 'b');
                        bChecked = true;
                    } else if($.trim(elem[key]['halfab']) === "") { aChecked = false; bChecked = false; }

                });
                objectsInvolved = false;
            }

        },

        /*******************************************/
        // Handle experiment validation.            /
        /*******************************************/
        validateExperiment: function(type) {
            var new_error = null;
            var feedDisplay = $( "#feedbackDisplay" );
            var that = this;

            // Step 1: Check to make sure the user has entered a value for the number of nights.
            var ret_val = false;
            if( $( '#scheduleNumNights' ).val() && $.trim($( '#scheduleNumNights' ).val()) !== "") {
                ret_val = true;
            } else {
                ret_val = false;
                new_error = new SNError( 4, "Please fill in the number of nights!", $( '#scheduleNumNights' ), feedDisplay, 0);
                new_error.displayError();
            }

            // Step 2: Check to make sure user has entered values for the nights and number of hours
            // Loop through each scheduled night text field
            if (ret_val != false) {
                var schNightHours_tf = $( '[id^="schNight"], [id^="schNHours"]' );

                $.each(schNightHours_tf, function(){
                    if($(this).val() && $.trim($(this).val()) !== "") {
                        ret_val = true;
                    } else {
                        ret_val = false;
                        new_error = new SNError( 4, "Please fill in the nights and the number of hours!", $(this), feedDisplay, 0);
                        new_error.displayError();
                        return false;
                    }
                });
            }


            // Step 3: Check to make sure the user entered a valid Experiment name
            if (ret_val != false) {
                var exp_name = $( '#experimentName' );
                if (exp_name.val() && $.trim(exp_name.val()) !== "")
                    ret_val = true;
                else {
                    ret_val = false;
                    new_error = new SNError( 4, "Please fill in an experiment name.", exp_name, $('#saveNewExperimentDialog').find('.snerror'), 0);
                    new_error.displayError();
                    return false;
                }
            }

            // Step 4: Check to make sure the experiment name isn't a duplicate
            if (ret_val != false) {
                var $exp_name = $( '#experimentName' );
                if($.inArray($exp_name.val(), GLOBAL.master_experiments_array['names']) > 0 && !updating && !addingNew) {
                    ret_val = false;
                    new_error = new SNError( 4, "You can't have duplicate experiment names.", $exp_name, $('#saveNewExperimentDialog').find('.snerror'), 0);
                    new_error.displayError();
                    return false;
                } else {
                    ret_val = true;
                }
            }


            // if(ret_val != false) {
            //     // Step 3: Make sure none of the redshift values are 0
            //     var redshiftVals = $( '[id^="redshifttf"]' );

            //     $.each(redshiftVals, function(){
            //         if($(this).val() && $.trim($(this).val()) !== "0") {
            //             ret_val = true;
            //         } else if( $.trim($(this).val()) === "0" ){
            //             ret_val = false;
            //             new_error = new SNError( 4, "Please make sure the redshift values are not 0!", $(this), feedDisplay, 0);
            //             new_error.displayError();
            //             return false;
            //         }
            //     });
            // }

            if (type === 'objects') {
                // Check to make sure the user has either selected a saved experiment or has saved a new experiment - dropdown cannot be --None--
                var selectedExpName = $( "#myExperimentSelector option" ).filter( ":selected" ).val();
                if (selectedExpName !== 'none') {
                    ret_val = true;
                    feedDisplay.empty();
                    $( '#scheduleNumNights' ).css( {'border-color':'', 'box-shadow':''} );
                }
                else {
                    ret_val = false;
                    new_error = new SNError( 4, "Please make sure you select an experiment from the 'My Configurations' dropdown above or save a new experiment in Configurations before saving the object information.", null, feedDisplay, 0);
                    new_error.displayError();
                    return false;
                }

                console.log("RETVAL " + ret_val);
                if(ret_val != false) {
                    // Make sure all of the supplementary fields are filled out
                    var supFields = $( '[id^="magtf"], [id^="redshifttf"], [id^="bpeaktf"], [id^="obsgaptf"], [id^="obstimestf"], [id^="prioritytf"]' );

                    $.each(supFields, function(){
                        if($(this).val() && $.trim($(this).val()) !== "") {
                            ret_val = true;
                            feedDisplay.empty();
                            $(this).css( {'border-color':'', 'box-shadow':''} );
                        } else {
                            ret_val = false;
                            new_error = new SNError( 4, "Please make sure all of the supplementary fields have been filled out.", $(this), feedDisplay, 0);
                            new_error.displayError();
                            return false;
                        }
                    });
                }
            }

            return ret_val;
        },

        validateScheduler: function(type, objText) {
            var that = this;
            var new_error = null;
            var planSuccess = true;
            var feedDisplay = $( "#feedbackDisplay" );
            var tempObjText = "";


            if (type == null) {
                // Handle making sure the general options fields are not empty
                var $genOptions = $( "#scheduleGeneralOptions input[type='text']" );
                that._clearTFCSS($genOptions);
                feedDisplay.empty();

                if(planSuccess) {
                    $genOptions.each(function() {
                        if($.trim($(this).val()) === ""){
                            new_error = new SNError( 4, "Please fill in the number of hours and the nights!", $(this), feedDisplay, 0);
                            new_error.displayError();

                            planSuccess = false;
                        }
                    });
                }

                // if(planSuccess) {
                //     // Handle the number of nights being consistent
                //     if($.trim($( "#scheduleObsDates" ).val()).split(/,\s*/).length > parseInt($( "#scheduleNumNights" ).val()) || $.trim($( "#scheduleObsDates" ).val()).split(/,\s*/).length < parseInt($( "#scheduleNumNights" ).val())) {
                //         new_error = new SNError( 4, "The number of nights and the nights scheduled don't match!", $("#scheduleNumNights, #scheduleObsDates"), feedDisplay, 0);
                //         new_error.displayError();

                //         planSuccess = false;
                //     }
                // }

                //Handle checking each object to make sure all the supplemental info has been entered.
                for(var key in that.master_objects_array) {
                    if(that.master_objects_array.hasOwnProperty(key)) {
                        if((that.master_objects_array[key].priority && that.master_objects_array[key].priority !== " " && that.master_objects_array[key].priority.length > 0)
                            && (that.master_objects_array[key].bpeak && that.master_objects_array[key].bpeak !== " " && that.master_objects_array[key].bpeak.length > 0)
                            && (that.master_objects_array[key].obsgap && that.master_objects_array[key].obsgap !== " " && that.master_objects_array[key].obsgap.length > 0)
                            && (that.master_objects_array[key].obstimes && that.master_objects_array[key].obstimes !== " " && that.master_objects_array[key].obstimes.length > 0))
                        {

                        } else {
                            that.errorObjs.push(key);
                            planSuccess = false;
                        }
                    }
                }

                if (objText != null) {
                    tempObjText = objText;
                    that.validateScheduler("save", tempObjText);
                }
            }

            if(type === "save") {
                var objSupInfoTf = $( "#scheduleObjectView" ).find('div div:last ul li input[type="text"]');
                that._clearTFCSS(objSupInfoTf);
                var msg_counter = 0;
                //console.log(that.master_objects_array[objText]);
                objSupInfoTf.each(function() {
                    //console.log($(this).val() + "test");
                    if($.trim($(this).val()) === "") {
                        new_error = new SNError( 4, " Please fill in the empty supplemental fields.", $(this), feedDisplay, 0);
                        if(msg_counter > 0) {
                        }

                        new_error.displayError();
                        msg_counter++;

                        planSuccess = false;
                    }
                });
                // if the msg_counter is 0, that means there was no errors found in the fields, so remove the object from the error objects
                if(msg_counter === 0) {
                    // Make sure to check if the object is in the error list (HAS TO HAVE AN INDEX GREATER THAN OR EQUAL TO 0!!)
                    if($.inArray(objText, that.errorObjs) >= 0)
                        that.errorObjs.splice($.inArray(objText, that.errorObjs), 1);
                }
            }

            return planSuccess;

        },

        validateSearch: function(type) {
            var that = this;
            $( "#searchSupernova" ).find( '.snerror' ).text("");

            switch(type){
                case 0:
                    var browse_all = $( "#browse_all" );
                    if (!(browse_all.is(':checked'))) {
                        var new_error = new SNError( 1, "Please check the \'Browse All\' checkbox.", null, browse_all.parent().next( "label" ), 0 );
                        new_error.displayError();
                        return false;
                    } else {
                        return true;
                    }
                    break;
                case 1:
                    /* Clear previous error markings. */
                    var name_field = $( "#obj_name" );
                    that._clearTFCSS(name_field);

                    if( name_field.val() && name_field.val() != ""){
                        return true;
                    } else {
                        var new_error = new SNError( 1, "Please fill in the name.", name_field, name_field.parent().next( "label" ), 0 );
                        new_error.displayError();
                        return false;
                    }
                    break;
                case 2:
                    var radecbool = true;
                    /* Clear previous error settings. */
                    var radec_field = $( "#searchSupernovaRADec > input[type='text']" );
                    that._clearTFCSS(radec_field);

                    radec_field.each(function() {
                        if($(this).val() === "") {
                            var new_error = new SNError( 1, "Please fill in the " + $(this).prev("label").text(), $(this), $(this).next( "label.snerror" ), 0 );
                            new_error.displayError();
                            radecbool = false;
                        }
                    });
                    return radecbool;
                    break;
                case 3:
                    if($('#supernovaAdvValue').val() && $('#supernovaAdvValue').val() !== "")
                        return true;
                    else {
                        var new_error = new SNError( 2, "Please enter a value.", $('#supernovaAdvValue'), $(this).parent().next("label"), 0);
                        new_error.displayError();
                        return false;
                    }
            }
        },

        /* Small validation function to check for duplicated list names when creating a new list. */
        validateNewList: function() {
            var that = this;
            if($.inArray($( "#newListName" ).val(), listNamesArr) >= 0){
                var new_error = new SNError( 2, "You can't have two lists with the same name. Please enter a different name.", $('#newListName'), $('#newListName').parent().next( "label" ), 0);
                new_error.displayError();
                return false;
            } else
                return true;
        }

    });

})(jQuery, window, document);

function initSupernovaeExt(){

    $(document).on({
        'supernovaeopen': function(event, ui) {
            $(window).trigger("resize.responsive");     //trigger the resize responsive event on the window so the fluidDialog can act upon open
            fluidDialog();
        },
        'supernovaeclose': function(event, ui) {
            $(window).off("resize.responsive");
        }
    }, '.ui-dialog');

    $("#supernovae").button({
        label:"SNeT v0.2",
        icons: {primary:"ui-icon-snicon"}
    });

    $("#supernovae_dialog").supernovae();

    $("#supernovae").button().on("click.showSNExt", function(){
        $("#supernovae_dialog").supernovae("open");
    });
}

function fluidDialog() {
    var $visible = $(".ui-dialog:visible");

    $visible.each(function () {
        var $this = $(this);
        var dialog = $this.find(".ui-dialog-content").data("supernovae");

        if (dialog.options.maxWidth && dialog.options.width) {
            // fix maxWidth bug
            $this.css("max-width", dialog.options.maxWidth);
            //reposition dialog
            dialog.option("position", dialog.options.position);
        }

        if (dialog.options.fluid) {
            // namespace window resize
            $(window).on("resize.responsive", function () {
                var wWidth = $(window).width();
                var wHeight = $(window).height();
                //check window width against dialog width
                if (wWidth < dialog.options.maxWidth + 50) {
                    // keep dialog from filling entire screen
                    $this.css("width", "90%");
                }
                //if (wHeight < $("#supernovae_dialog").parent().outerHeight() + 20) {
                if (wHeight < dialog.options.height + 50) {
                    $this.css({ "height": "90%", "overflow": "hidden" });
                    $(".displaySupernovaContent").css({"height": ($this.outerHeight() - 140) + "px"});
                }
                //reposition dialog
                dialog.option("position", dialog.options.position);
            });
        }
    });
}
