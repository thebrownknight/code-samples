<?php
/*
	Plugin Name: Paramount Sections
	Plugin URI: http://thebrownknight.com/paramount-sections
	Tags: jquery, important, content
	Description: Create a highly customizable content area.
	Author: Nikhil Venkatesh
	Version: 0.1
	Author URI: http://thebrownknight.com
	Copyright 2014 Nikhil Venkatesh
*/

class paramount_secs {
	function paramount_secs() {
		if (is_admin()) {
			// Header Styles
			add_action( 'wp_enqueue_scripts', 'ps_scripts' );
		}
	}

	function ps_scripts() {
		// Scripts
		wp_enqueue_scripts( 'jquery' );
		wp_enqueue_scripts( 'jqueryparamountsections', paramount_secs::get_plugin_directory() . '/js/jquery.paramountsections.0.1.js', array( 'jquery' ) );
	}

	function get_plugin_directory() {
		return WP_PLUGIN_URL . '/paramount-sections';
	}
};

require_once('inc/paramount_sections_admin.php');

// Initialize the plugin.
$para_sec = new paramount_secs();

// Register the widget by creating a function
add_action('widgets_init', create_function('', 'return register_widget("paramount_sections");'));