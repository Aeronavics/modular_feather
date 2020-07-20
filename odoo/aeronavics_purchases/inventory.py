from odoo import models, fields, api

class productInherit(models.Model):
    _inherit = 'product.template'
    
    material = fields.Char(string='Material')
    colour = fields.Char(string='Colour')
    production_method = fields.Char(string='Production Method')
    thread = fields.Selection(string='Thread', selection=[('yes','Yes'),('no','No'),('na','N/A')], required=True, default='na')
