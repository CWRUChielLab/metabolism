#!/usr/Rscript
#
# Subroutine for defining plot attributes common to both
# _plot_diffusion.R and _plot_diffusion_batch.R


# List the names of the diffusion data types in the order
# they should be plotted
diffusion_types = c("dx_actual", "dx_ideal", "dy_actual", "dy_ideal")


# Define the plot titles
plot_title = list()
if (output_type == "latex")
{
   plot_title[which(diffusion_types == "dx_actual")] = "Displacement in the\n$x$-Dimension with Collisions"
   plot_title[which(diffusion_types == "dx_ideal")]  = "Displacement in the\n$x$-Dimension without Collisions"
   plot_title[which(diffusion_types == "dy_actual")] = "Displacement in the\n$y$-Dimension with Collisions"
   plot_title[which(diffusion_types == "dy_ideal")]  = "Displacement in the\n$y$-Dimension without Collisions"
} else {
   plot_title[which(diffusion_types == "dx_actual")] = "Displacement in the\nx-Dimension with Collisions"
   plot_title[which(diffusion_types == "dx_ideal")]  = "Displacement in the\nx-Dimension without Collisions"
   plot_title[which(diffusion_types == "dy_actual")] = "Displacement in the\ny-Dimension with Collisions"
   plot_title[which(diffusion_types == "dy_ideal")]  = "Displacement in the\ny-Dimension without Collisions"
}


# Define the plot color schemes
plot_color = list()
plot_color[which(diffusion_types == "dx_actual")] = "#249800"  # green
plot_color[which(diffusion_types == "dx_ideal")]  = "#237A09"  # darkgreen
plot_color[which(diffusion_types == "dy_actual")] = "#0020AB"  # blue
plot_color[which(diffusion_types == "dy_ideal")]  = "#001055"  # darkblue


# Set completion flag
diffusion_plots_prepped = TRUE


# End

