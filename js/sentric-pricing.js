/*******************************
 * jQuery Sentric Pricing plugin designed to create the flow for the pricing page.
 * Author: Nikhil Venkatesh
 * Date: 03/05/15
 *******************************/

;(function ( $, window, document, undefined ) {
	"use strict";

	var pluginName = "sentricPricing",
		defaults = {
			debug: false
		};

	/*
	 * The plugin constructor.
	 */
	function sentricPricing( element, options ) {
		this.element = element;
		this.options = $.extend( {}, defaults, options );

		this._defaults = defaults;
		this._name = pluginName;

		// Set up the plugin.
		this.init();
	}

	sentricPricing.prototype = {
		/*
		 * Initialization function.
		 * Set up the Observer pattern.
		 * Basically we are watching for user's interactions with the pricing calculator
		 */
		init: function() {
			// this.element lets you have access to the element used to instantiate the plugin
			var observer = $( {} );
			$.subscribe = observer.on.bind(observer);
			$.unsubscribe = observer.off.bind(observer);
			$.publish = observer.trigger.bind(observer);

			/* Set up variables to maintain throughout instance */
			// An array that stores all the buttons/inputs that are causing errors.
			this.errorFields = {};

			this.setUpListeners();
		},

		/*
		 * Set up the event listeners for the various buttons on the calculator.
		 */
		setUpListeners: function() {
			var self = this;
			var $elem = $(this.element);
			var $twoStep = $elem.find('.two-column-step');
			var $pricingPackage = $elem.find('.pricing-package');
			var $servicePackage = $elem.find('.service-package');
			var $submitButton = $twoStep.find('.submit-button');
			var $pricingCPButton = $pricingPackage.find('.choose-plan-button');
			var $serviceCPButton = $servicePackage.find('.choose-plan-button');

			// First handle the submit button clicks
			$submitButton.on('click', function(event){
				var $this = $(this);
				self.handleSubmitButton(event, $this);
			});

			// Next handle the Pricing Choose Plan button clicks
			$pricingCPButton.on('click', function(event){
				var $this = $(this);
				self.handlePricingCPButton(event, $this);
			});

			// Lastly handle the Service Choose Plan button clicks
			$serviceCPButton.on('click', function(event){
				var $this = $(this);
				self.handleServiceCPButton(event, $this);
			});

		},

		/*
		 * Submit button click handler (for two-column steps)
		 * @param event - the event being triggered when clicked on
		 */
		handleSubmitButton: function( event, elem ) {
			var self = this;
			event.preventDefault();

			// Check which step we're in
			// If it's the first step, validate the employees
			// If it's the third step, make sure the first two steps have been filled out
			var $step = elem.parents('div[id^="step_"]'),
				stepNumber = $step.attr('id').split('_')[1],
				$inputField = $step.find('.input-field-container');

			if ( stepNumber === "1" ) {
				var $input = $inputField.find('.pricing-input');
				// self.validateEmployees( $input, $inputField );
				self.validatePreviousSteps( parseInt(stepNumber) );
				if ( this.calculatedResults === true ) {
					this.calculateResults();
					this.scrollToStep(5);
				} else {
					if ( Object.keys(this.errorFields).length === 0 )
						this.scrollToStep(2);
				}
			} else if ( stepNumber === "3" ) {
				var $dropdown = $inputField.find('.pricing-dropdown');
				self.validatePreviousSteps( parseInt(stepNumber) );
				//self.validatePayroll( $dropdown, $inputField );
				if ( this.calculatedResults === true ) {
					this.calculateResults();
					this.scrollToStep(5);
				} else {
					if ( Object.keys(this.errorFields).length === 0 )
						this.scrollToStep(4);
				}
			}
		},

		/*
		 * Pricing Choose Plan button click handler (for package selection)
		 * @param event - the event being triggered when clicked on
		 * @param elem - the specific Choose Plan button that was clicked
		 */
		handlePricingCPButton: function( event, elem ) {
			var self = this;
			event.preventDefault();

			// We have 3 different plans here, need to add/remove "selected" classes
			$('.pricing-package').find('.choose-plan-button').removeClass('selected');
			self.validatePreviousSteps( 1 );

			if ( this.errorFields['1'] === "error" ) {

			} else {
				elem.addClass('selected');
				self.showSuccess( $('.pricing-package') );
				if ( this.calculatedResults === true ) {
					self.calculateResults();
					self.scrollToStep(5);
				} else {
					self.scrollToStep(3);
				}
			}

		},

		/*
		 * Pricing Choose Plan button click handler (for package selection)
		 * @param event - the event being triggered when clicked on
		 */
		handleServiceCPButton: function( event, elem ) {
			var self = this;
			event.preventDefault();

			// We have 2 different plans here, need to add/remove "selected" classes
			$('.service-package').find('.choose-plan-button').removeClass('selected');
			self.validatePreviousSteps( 3 );

			if ( Object.keys(this.errorFields).length === 0 ) {
				elem.addClass('selected');
				self.showSuccess( $('.service-package') );
				self.calculateResults();
				self.scrollToStep(5);
			}
		},

		/*
		 * Final results calculation.
		 * Equation: ((package value * # of employees * 12) + ($2.50 * # of employees * annual pay cycles)) / 12
		 */
		calculateResults: function() {
			var self = this,
				numEmployees = parseInt($('input[name="employeeNumber"]').val()),
				pricingPackageValue = parseInt($('.pricing-package .choose-plan-button.selected').parents('.package-container').data("price")),
				pricingPackage = $('.pricing-package .choose-plan-button.selected').data("package"),
				payrollFrequency = parseInt($('select[name="payroll_frequency"]').val()),
				payrollText = $('select[name="payroll_frequency"] option:selected').text(),
				servicePackage = $('.service-package .choose-plan-button.selected').data("package");

			// Check what service package the user selected.
			// If they selected 'Concierge', show the special text.
			if ( servicePackage === "Concierge" ) {
				self.showConcierge();
			}
			else {
				self.hideConcierge();
				// Normalize the number of employees
				numEmployees = numEmployees < 35 ? 35 : numEmployees;

				var finalCost = ((pricingPackageValue * numEmployees * 12) + (2.5 * numEmployees * payrollFrequency)) / 12;
				finalCost = finalCost.toFixed(2);

				self.showResults( finalCost, pricingPackage, payrollText, numEmployees, servicePackage );
			}
		},

		/*
		 * Method to show the concierge text and hide everything else.
		 */
		showConcierge: function() {
			if ( $('.concierge-text').css('display') === 'none' )
				this.manipulateCalcFields();
		},

		/*
		 * Method to show the concierge text and hide everything else.
		 */
		hideConcierge: function() {
			if ( $('.concierge-text').css('display') === 'block' )
				this.showCalculationFields();
		},

		/*
		 * Method to show the results in the input fields.
		 */
		showResults: function( finalCost, pricingPackage, payrollText, numEmployees, servicePackage ) {
			$('input[name="cost_output"]').val("$" + this.formatPricing(finalCost));
			$('input[name="sentric_sales_force_plan"]').val(pricingPackage);
			$('input[name="payroll_text"]').val(payrollText);
			$('input[name="num_employees"]').val(numEmployees);
			$('input[name="support_package"]').val(servicePackage);

			// Set this boolean so that anytime they edit any of the fields,
			// we know to recalculate the results.
			this.calculatedResults = true;
		},

		/*
		 * Validate all previous steps to ensure rest of
		 * the calculator is filled out,
		 */
		validatePreviousSteps: function( stepNumber ) {
			var self = this,
				tempStepNumber = stepNumber;
			while ( tempStepNumber > 0 ) {
				var $stepContainer = $('div[id="step_' + tempStepNumber + '"]');
				switch ( tempStepNumber ) {
					case 1:
						var $elemContainer = $stepContainer.find('.input-field-container'),
							$elem = $elemContainer.find('.pricing-input');
						self.validateEmployees( $elem, $elemContainer );
						break;
					case 2:
						self.validatePricingPackage();
						break;
					case 3:
						var $elemContainer = $stepContainer.find('.input-field-container'),
							$elem = $elemContainer.find('.pricing-dropdown');
						self.validatePayroll( $elem, $elemContainer );
						break;
					case 4:
						self.validateServicePackage();
						break;
					default:
						break;
				}
				tempStepNumber--;
			}
			var scrollNum = parseInt(Object.keys(this.errorFields)[0]);

			// console.log( this.errorFields );

			if ( Object.keys(this.errorFields).length > 0 )
				self.scrollToStep( scrollNum );
		},

		/*
		 * Validation of employees field.
		 */
		validateEmployees: function( elem, elemContainer ) {
			var $errorContainer = elemContainer.next('.error-message-container');
			if ( elem.data("validation") === "employee" && elem.val().trim() === "" ) {
				this.errorFields['1'] = "error";

				this.showError( elemContainer.parents('div[id^="step_"]') );
			} else {
				$errorContainer.find('> p').hide();
				if ( parseInt(elem.val()) < 35 ) {
					if ( $errorContainer.find('> p').css('display') !== 'block' )
						$errorContainer.find('> p').fadeIn(200);
				}
				this.showSuccess( elemContainer.parents('div[id^="step_"]') );
				delete this.errorFields['1'];
			}
		},

		/*
		 * Validation of Pricing Packages section.
		 */
		validatePricingPackage: function () {
			if ( $('.pricing-package').find('.choose-plan-button.selected').length <= 0 ) {
				this.errorFields['2'] = "error";
				this.showError( $('.pricing-package') );
			} else {
				delete this.errorFields['2'];
				this.showSuccess ( $('.pricing-package') );
			}
		},

		/*
		 * Validation of process payroll field.
		 */
		validatePayroll: function( elem, elemContainer ) {
			var $errorContainer = elemContainer.next('.error-message-container'),
				payrollValue = $( 'select[name="payroll_frequency"]' ).val();

			if ( elem.data("validation") === "payroll" && ( payrollValue === null || payrollValue.trim() === "" ) ) {
				this.errorFields['3'] = "error";
				$errorContainer.empty().append('<p><em>Please select a payroll frequency.</em></p>');
				this.showError( elemContainer.parents('div[id^="step_"]') );
			} else {
				$errorContainer.empty();
				delete this.errorFields['3'];
				this.showSuccess( elemContainer.parents('div[id^="step_"]') );
			}
		},

		/*
		 * Validation of Service Packages section.
		 */
		validateServicePackage: function () {
			if ( $('.service-package').find('.choose-plan-button.selected').length <= 0 ) {
				this.errorFields['4'] = "error";
				this.showError( $('.service-package') );
			} else {
				delete this.errorFields['4'];
				this.showSuccess ( $('.service-package') );
			}
		},

		/*
		 * Show the success check if we pass the criteria.
		 */
		showError: function( stepContainer ) {
			stepContainer.find('.neutral-check, .success-check').css('display', 'none');
			stepContainer.find('.error-check').css('display', 'inline-block');
		},

		/*
		 * Show the success check if we pass the criteria.
		 */
		showSuccess: function( stepContainer ) {
			stepContainer.find('.neutral-check, .error-check').css('display', 'none');
			stepContainer.find('.success-check').css('display', 'inline-block');
		},

		/*
		 * Utility method to scroll to the correct step.
		 */
		scrollToStep: function( stepNumber ) {
			// console.log( stepNumber );
			$('html, body').animate({
			  scrollTop: $('#step_' + stepNumber).offset().top - 200
			}, 1000);
		},

		/*
		 *
		 */
		formatPricing: function( price ) {
			var strReverse = function(str) {
				return str.split("").reverse().join("");
			};
			var newPrice = "",
				splitPrice = price.split('.'),
				reversedPrice = strReverse(splitPrice[0]);
			for ( var index = 0; index < reversedPrice.length; ++index ) {
				newPrice += reversedPrice[index];
				if ( (index + 1) % 3 === 0 ) {
					newPrice += ",";
				}
			}

			if ( newPrice[newPrice.length - 1] === "," )
				newPrice = newPrice.slice(0, -1);

			return strReverse(newPrice) + "." + splitPrice[1];
		},

		/*
		 * Utility method to hide all calculation fields.
		 */
		manipulateCalcFields: function() {
			$('.cost-output > div:not(".concierge-text")').hide();
			$('.pricing-summary').hide();
			$('.concierge-text').fadeIn(200);
		},

		/*
		 * Utility method to hide all calculation fields.
		 */
		showCalculationFields: function() {
			$('.cost-output > div:not(".concierge-text")').show();
			$('.pricing-summary').show();
			$('.concierge-text').hide();
		}
	};

	// The actual plugin instantiation
	// A wrapper create against multiple instantiations
	$.fn[pluginName] = function ( options ) {
		return this.each( function () {
			if ( !$.data(this, "plugin_" + pluginName) ) {
				$.data(this, "plugin_" + pluginName,
					new sentricPricing( this, options ));
			}
		});
	};

}( jQuery, window, document ))