/*
	Paramount Sections widget scripts.
*/
jQuery.noConflict();

/** Fire up jQuery - let's dance!
 */
jQuery(document).ready(function($){

/** Sortable Lists
	----------------------------------------------- */
	// AJAX Add New <list-item>
	function ps_sortable_list_add_item(action_id, items) {

		var columnID = items.attr('rel'),
			numArr = items.find('li').map(function(i, e){
				return $(e).attr("rel");
			});

		var maxNum = Math.max.apply(Math, numArr);
		if (maxNum < 1 ) { maxNum = 0; }
		var newNum = maxNum + 1;

		var data = {
			action: 'ps_block_'+action_id+'_add_new',
			//security: $('#aqpb-nonce').val(),
			count: newNum,
			column_id: columnID
		};

		$.post(ajaxurl, data, function(response) {
			// var check = response.charAt(response.length - 1);

			// //check nonce
			// if(check == '-1') {
			// alert('An unknown error has occurred');
			// } else {
				items.append(response);	// add the new question
			// }

		});
	}

	// Initialise sortable list fields
	function ps_sortable_list_init() {
		$('.ps-sortable-list').sortable({
			containment: "document",
			placeholder: "widget-placeholder",
			cursor: "move"
		});
	}
	/*** Initialize the sortable question list ***/
	ps_sortable_list_init();

	$('ul.blocks').bind('sortstop', function() {
		ps_sortable_list_init();
	});


	$(document).on('click', 'a.ps-sortable-add-new', function() {
		var action_id = $(this).attr('rel'),
			items = $(this).parent().children('ul.ps-sortable-list');

		ps_sortable_list_add_item(action_id, items);
		ps_sortable_list_init();
		return false;
	});

	// Delete Sortable Item
	$(document).on('click', '.ps-sortable-list a.sortable-delete', function() {
		// var $parent = $(this.parentNode.parentNode.parentNode);
		// $parent.children('.block-tabs-tab-head').css('background', 'red');
		// $parent.slideUp(function() {
		// $(this).remove();
		// }).fadeOut('fast');
		var $parent = $(this).parents('li.sortable-item');
		$parent.slideUp(function() {
			$(this).remove();
		}).fadeOut('fast');
		return false;
	});

	// Open/Close Sortable Item
	$(document).on('click', '.ps-sortable-list .sortable-handle a', function() {
		var $clicked = $(this);

		$clicked.addClass('sortable-clicked');

		$clicked.parents('.ps-sortable-list').find('.sortable-body').each(function(i, el) {
			if($(el).is(':visible') && $(el).prev().find('a').hasClass('sortable-clicked') === false) {
				$(el).slideUp();
			}
		});
		$(this.parentNode.parentNode.parentNode).children('.sortable-body').slideToggle();

		$clicked.removeClass('sortable-clicked');

		return false;
	});

	// Handle select dropdown options changes
	$(document).on('change', '.column-type-dd', function() {
		var selectedOption = $(this).val();
		switch(selectedOption){
			case 'standardcolumn':
				$('.image-with-content-section, .blockquote-section, .form-section, .simple-image-section').fadeOut(500);
				$('.standard-column-section').fadeIn(500);
				break;
			case 'simpleimage':
				$('.standard-column-section, .blockquote-section, .form-section, .image-with-content-section').fadeOut(500);
				$('.simple-image-section').fadeIn(500);
				break;
			case 'imagewithcontent':
				$('.standard-column-section, .blockquote-section, .form-section, .simple-image-section').fadeOut(500);
				$('.image-with-content-section').fadeIn(500);
				break;
			case 'blockquote':
				$('.standard-column-section, .image-with-content-section, .form-section, .simple-image-section').fadeOut(500);
				$('.blockquote-section').fadeIn(500);
				break;
			case 'form':
				$('.standard-column-section, .blockquote-section, .image-with-content-section, .simple-image-section').fadeOut(500);
				$('.form-section').fadeIn(500);
				break;
			default:
				break;
		}
	});

});